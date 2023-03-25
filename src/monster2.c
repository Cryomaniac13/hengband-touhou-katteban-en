/* File: monster2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: misc code for monsters */

#include "angband.h"

#define HORDE_NOGOOD 0x01
#define HORDE_NOEVIL 0x02

cptr horror_desc[MAX_SAN_HORROR] =
{
#ifdef JP
	"忌まわしい",
	"底知れぬ",
	"ぞっとする",
	"破滅的な",
	"冒涜的な",

	"いやな",
	"恐ろしい",
	"不潔な",
	"容赦のない",
	"おぞましい",

	"地獄の",
	"身の毛もよだつ",
	"地獄の",
	"忌まわしい",
	"悪夢のような",

	"嫌悪を感じる",
	"罰当たりな",
	"恐い",
	"不浄な",
	"言うもおぞましい",
#else
	"abominable",
	"abysmal",
	"appalling",
	"baleful",
	"blasphemous",

	"disgusting",
	"dreadful",
	"filthy",
	"grisly",
	"hideous",

	"hellish",
	"horrible",
	"infernal",
	"loathsome",
	"nightmarish",

	"repulsive",
	"sacrilegious",
	"terrible",
	"unclean",
	"unspeakable",
#endif

};

cptr funny_desc[MAX_SAN_FUNNY] =
{
#ifdef JP
	"間抜けな",
	"滑稽な",
	"ばからしい",
	"無味乾燥な",
	"馬鹿げた",

	"笑える",
	"ばかばかしい",
	"ぶっとんだ",
	"いかした",
	"ポストモダンな",

	"ファンタスティックな",
	"ダダイズム的な",
	"キュビズム的な",
	"宇宙的な",
	"卓越した",

	"理解不能な",
	"ものすごい",
	"驚くべき",
	"信じられない",
	"カオティックな",

	"野性的な",
	"非常識な",
#else
	"silly",
	"hilarious",
	"absurd",
	"insipid",
	"ridiculous",

	"laughable",
	"ludicrous",
	"far-out",
	"groovy",
	"postmodern",

	"fantastic",
	"dadaistic",
	"cubistic",
	"cosmic",
	"awesome",

	"incomprehensible",
	"fabulous",
	"amazing",
	"incredible",
	"chaotic",

	"wild",
	"preposterous",
#endif

};

cptr funny_comments[MAX_SAN_COMMENT] =
{
#ifdef JP
  /* nuke me */
	"最高だぜ！",
	"うひょー！",
	"いかすぜ！",
	"すんばらしい！",
	"ぶっとびー！"
#else
	"Wow, cosmic, man!",
	"Rad!",
	"Groovy!",
	"Cool!",
	"Far out!"
#endif

};


/*
 * Set the target of counter attack
 */
void set_target(monster_type *m_ptr, int y, int x)
{
	m_ptr->target_y = y;
	m_ptr->target_x = x;
}


/*
 * Reset the target of counter attack
 */
void reset_target(monster_type *m_ptr)
{
	set_target(m_ptr, 0, 0);
}


/*
 *  Extract monster race pointer of a monster's true form
 */
/*:::モンスターの種族を知りたいとき、m_ptr->r_idxをそのまま使ったらカメレオンが変身先の種族になるのでここで修正する*/
///mod140629 カメレオンを妖怪狐として再実装　カメレオンの王はなし
monster_race *real_r_ptr(monster_type *m_ptr)
{
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	/* Extract real race */
	if (m_ptr->mflag2 & MFLAG2_CHAMELEON)
	{
//		if (r_ptr->flags1 & RF1_UNIQUE)
//			return &r_info[MON_CHAMELEON_K];
//		else
		return &r_info[MON_CHAMELEON];
	}
	else
	{
		return r_ptr;
	}
}


/*
 * Delete a monster by index.
 *
 * When a monster is deleted, all of its objects are deleted.
 */
/*:::m_list[i]のモンスターを削除する*/
void delete_monster_idx(int i)
{
	int x, y;

	monster_type *m_ptr = &m_list[i];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	s16b this_o_idx, next_o_idx = 0;


	/* Get location */
	y = m_ptr->fy;
	x = m_ptr->fx;

	//v1.1.58 モンスター専用BGMが鳴っていた場合リセット
	if (m_ptr->r_idx == music_r_idx)
	{
		music_r_idx = 0;
		flag_update_floor_music = TRUE;
	}

	//v1.1.77 お燐(専用性格)の追跡対象モンスターが消えたら追跡中断
	if (is_special_seikaku(SEIKAKU_SPECIAL_ORIN) && i == p_ptr->magic_num1[0])
	{
		p_ptr->magic_num1[0] = 0;
	}

	/* Hack -- Reduce the racial counter */
	/*:::モンスターのフロア存在数を1減らす*/
	real_r_ptr(m_ptr)->cur_num--;

	/* Hack -- count the number of "reproducers" */
	/*:::増殖モンスター用カウンター？も1減らす*/
	if (r_ptr->flags2 & (RF2_MULTIPLY)) num_repro--;

	/*:::モンスターの一時ステータスを全て解除する*/
	if (MON_CSLEEP(m_ptr)) (void)set_monster_csleep(i, 0);
	if (MON_FAST(m_ptr)) (void)set_monster_fast(i, 0);
	if (MON_SLOW(m_ptr)) (void)set_monster_slow(i, 0);
	if (MON_STUNNED(m_ptr)) (void)set_monster_stunned(i, 0);
	if (MON_CONFUSED(m_ptr)) (void)set_monster_confused(i, 0);
	if (MON_MONFEAR(m_ptr)) (void)set_monster_monfear(i, 0);
	if (MON_INVULNER(m_ptr)) (void)set_monster_invulner(i, 0, FALSE);

	//v1.1.94 追加一時ステータス
	if (MON_DEC_ATK(m_ptr)) (void)set_monster_timed_status_add(MTIMED2_DEC_ATK, i, 0);
	if (MON_DEC_DEF(m_ptr)) (void)set_monster_timed_status_add(MTIMED2_DEC_DEF, i, 0);
	if (MON_DEC_MAG(m_ptr)) (void)set_monster_timed_status_add(MTIMED2_DEC_MAG, i, 0);
	if (MON_DRUNK(m_ptr)) (void)set_monster_timed_status_add(MTIMED2_DRUNK, i, 0);
	if (MON_NO_MOVE(m_ptr)) (void)set_monster_timed_status_add(MTIMED2_NO_MOVE, i, 0);
	if (MON_BERSERK(m_ptr)) (void)set_monster_timed_status_add(MTIMED2_BERSERK, i, 0);


	/*:::モンスターのターゲット、HPバー表示フラグなどを解除する*/
	/* Hack -- remove target monster */
	if (i == target_who) target_who = 0;

	/* Hack -- remove tracked monster */
	if (i == p_ptr->health_who) health_track(0);

	if (pet_t_m_idx == i ) pet_t_m_idx = 0;
	if (riding_t_m_idx == i) riding_t_m_idx = 0;
	if (p_ptr->riding == i) p_ptr->riding = 0;

	/* Monster is gone */
	cave[y][x].m_idx = 0;


	/* Delete objects */
	/*:::アイテムを持っていればそのアイテムは消滅*/
	for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */

		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/*
		 * o_ptr->held_m_idx is needed in delete_object_idx()
		 * to prevent calling lite_spot()
		 */

		/* Delete the object */
		delete_object_idx(this_o_idx);
	}

	/*:::死んだのがペットのとき*/
	if (is_pet(m_ptr)) check_pets_num_and_align(m_ptr, FALSE);


	/* Wipe the Monster */
	(void)WIPE(m_ptr, monster_type);

	/* Count monsters */
	m_cnt--;

	/* Visual update */
	//v1.1.32 建物内からの抹殺に対応するために条件追加
	if (!character_icky)
		lite_spot(y, x);

	//ぬえがモンスターの消滅により変身可能になったかチェック
	//v2.0.5 変身可能チェックはprocess_player()の＠の手番開始のときに限定することにしここではフラグだけ立てる。
	//たぶんこれが想定しないときに呼ばれているのがクラッシュバグの原因
	//if(p_ptr->pclass == CLASS_NUE)
	//{
	//	check_nue_undefined();
	//}
	flag_nue_check_undefined = TRUE;

	/* Update some things */
	if (r_ptr->flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
		p_ptr->update |= (PU_MON_LITE);


}


/*
 * Delete the monster, if any, at a given location
 */
void delete_monster(int y, int x)
{
	cave_type *c_ptr;

	/* Paranoia */
	if (!in_bounds(y, x)) return;

	/* Check the grid */
	c_ptr = &cave[y][x];

	/* Delete the monster (if any) */
	if (c_ptr->m_idx) delete_monster_idx(c_ptr->m_idx);
}


/*
 * Move an object from index i1 to index i2 in the object list
 */
/*:::m_list[]の中でモンスターを移動する。移動先にいたモンスターは消える。*/
/*:::cave[][].m_idx, o_list[].held_m_idxにはm_list[]の添え字が使われてるのでこれも更新する。*/
/*:::同様に騎乗情報、ターゲット情報、モンスターステータス異常も更新する。*/
static void compact_monsters_aux(int i1, int i2)
{
	int y, x, i;

	cave_type *c_ptr;

	monster_type *m_ptr;

	s16b this_o_idx, next_o_idx = 0;


	/* Do nothing */
	if (i1 == i2) return;


	/* Old monster */
	m_ptr = &m_list[i1];

	/* Location */
	y = m_ptr->fy;
	x = m_ptr->fx;

	/* Cave grid */
	c_ptr = &cave[y][x];

	/* Update the cave */
	c_ptr->m_idx = i2;

	/* Repair objects being carried by monster */
	for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Reset monster pointer */
		o_ptr->held_m_idx = i2;
	}

	/* Hack -- Update the target */
	if (target_who == i1) target_who = i2;

	/* Hack -- Update the target */
	if (pet_t_m_idx == i1) pet_t_m_idx = i2;
	if (riding_t_m_idx == i1) riding_t_m_idx = i2;

	/* Hack -- Update the riding */
	if (p_ptr->riding == i1) p_ptr->riding = i2;

	/* Hack -- Update the health bar */
	if (p_ptr->health_who == i1) health_track(i2);

	/* Hack -- Update parent index */
	if (is_pet(m_ptr))
	{
		for (i = 1; i < m_max; i++)
		{
			monster_type *m2_ptr = &m_list[i];

			if (m2_ptr->parent_m_idx == i1)
				m2_ptr->parent_m_idx = i2;
		}
	}

	/* Structure copy */
	(void)COPY(&m_list[i2], &m_list[i1], monster_type);

	/* Wipe the hole */
	(void)WIPE(&m_list[i1], monster_type);

	for (i = 0; i < MAX_MTIMED; i++)
	{
		int mproc_idx = get_mproc_idx(i1, i);
		if (mproc_idx >= 0) mproc_list[i][mproc_idx] = i2;
	}
}


/*
 * Compact and Reorder the monster list
 *
 * This function can be very dangerous, use with caution!
 *
 * When actually "compacting" monsters, we base the saving throw
 * on a combination of monster level, distance from player, and
 * current "desperation".
 *
 * After "compacting" (if needed), we "reorder" the monsters into a more
 * compact order, and we reset the allocation info, and the "live" array.
 */
/*:::m_listを圧縮し、m_cntとm_maxを再設定する*/
/*:::sizeは強制圧縮量で、sizeが0でない場合＠から遠い低レベルなモンスターをランダムに消滅させる*/
void compact_monsters(int size)
{
	int		i, num, cnt;
	int		cur_lev, cur_dis, chance;

	/* Message (only if compacting) */
#ifdef JP
	if (size) msg_print("モンスター情報を圧縮しています...");
#else
	if (size) msg_print("Compacting monsters...");
#endif


	/* Compact at least 'size' objects */
	/*:::圧縮処理　size0のときはここに入らない*/
	/*:::基準レベルと距離に達しないモンスターを確率で削除し、削除数がsizeを超えるまで基準を狭めながら繰り返す*/
	for (num = 0, cnt = 1; num < size; cnt++)
	{
		/* Get more vicious each iteration */
		/*:::削除基準レベル　削除が進まないとどんどん高くなる*/
		cur_lev = 5 * cnt;

		/* Get closer each iteration */
		/*:::削除基準距離　削除が進まないとどんどん近くなる*/
		cur_dis = 5 * (20 - cnt);

		/* Check all the monsters */
		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];

			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			/* Paranoia -- skip "dead" monsters */
			/*:::死んだモンスターは無視。あとで消されるから。*/
			if (!m_ptr->r_idx) continue;

			/* Hack -- High level monsters start out "immune" */
			if (r_ptr->level > cur_lev) continue;

			if (i == p_ptr->riding) continue;

			/* Ignore nearby monsters */
			if ((cur_dis > 0) && (m_ptr->cdis < cur_dis)) continue;

			/* Saving throw chance */
			chance = 90;

			/* Only compact "Quest" Monsters in emergencies */
			if ((r_ptr->flags1 & (RF1_QUESTOR)) && (cnt < 1000)) chance = 100;

			/* Try not to compact Unique Monsters */
			/*:::上のそうだがなぜcontinueにしないのだろう*/
			if (r_ptr->flags1 & (RF1_UNIQUE)) chance = 100;

			/* All monsters get a saving throw */
			/*:::randの結果は0-99*/
			if (randint0(100) < chance) continue;

			/*:::名前をつけたペットが圧縮されたら日記に記述*/
			if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
			{
				char m_name[80];

				monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
				do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_COMPACT, m_name);
			}

			/* Delete the monster */
			delete_monster_idx(i);

			/* Count the monster */
			num++;
		}
	}


	/* Excise dead monsters (backwards!) */
	for (i = m_max - 1; i >= 1; i--)
	{
		/* Get the i'th monster */
		monster_type *m_ptr = &m_list[i];

		/* Skip real monsters */
		if (m_ptr->r_idx) continue;

		/* Move last monster into open hole */
		compact_monsters_aux(m_max - 1, i);

		/* Compress "m_max" */
		m_max--;
	}
}


/*
 * Delete/Remove all the monsters when the player leaves the level
 *
 * This is an efficient method of simulating multiple calls to the
 * "delete_monster()" function, with no visual effects.
 */
void wipe_m_list(void)
{
	int i;

	/* Hack -- if Banor or Lupart dies, stay another dead */
	if (!r_info[MON_BANORLUPART].max_num)
	{
		if (r_info[MON_BANOR].max_num)
		{
			r_info[MON_BANOR].max_num = 0;
			r_info[MON_BANOR].r_pkills++;
			r_info[MON_BANOR].r_akills++;
			if (r_info[MON_BANOR].r_tkills < MAX_SHORT) r_info[MON_BANOR].r_tkills++;
		}
		if (r_info[MON_LUPART].max_num)
		{
			r_info[MON_LUPART].max_num = 0;
			r_info[MON_LUPART].r_pkills++;
			r_info[MON_LUPART].r_akills++;
			if (r_info[MON_LUPART].r_tkills < MAX_SHORT) r_info[MON_LUPART].r_tkills++;
		}
	}

	/* Delete all the monsters */
	for (i = m_max - 1; i >= 1; i--)
	{
		monster_type *m_ptr = &m_list[i];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Monster is gone */
		cave[m_ptr->fy][m_ptr->fx].m_idx = 0;

		/* Wipe the Monster */
		(void)WIPE(m_ptr, monster_type);

	}

	/*
	 * Wiping racial counters of all monsters and incrementing of racial
	 * counters of monsters in party_mon[] are required to prevent multiple
	 * generation of unique monster who is the minion of player.
	 */

	/* Hack -- Wipe the racial counter of all monster races */
	for (i = 1; i < max_r_idx; i++) r_info[i].cur_num = 0;

	/* Reset "m_max" */
	m_max = 1;

	/* Reset "m_cnt" */
	m_cnt = 0;

	/* Reset "mproc_max[]" */
	for (i = 0; i < MAX_MTIMED; i++) mproc_max[i] = 0;

	/* Hack -- reset "reproducer" count */
	num_repro = 0;

	/* Hack -- no more target */
	target_who = 0;
	pet_t_m_idx = 0;
	riding_t_m_idx = 0;

	/* Hack -- no more tracking */
	health_track(0);
}


/*
 * Acquires and returns the index of a "free" monster.
 *
 * This routine should almost never fail, but it *can* happen.
 */
/*:::フロアの生きてるモンスターのリストであるm_list[]のうち開いてる添え字を返す*/
s16b m_pop(void)
{
	int i;


	/* Normal allocation */
	if (m_max < max_m_idx)
	{
		/* Access the next hole */
		i = m_max;

		/* Expand the array */
		m_max++;

		/* Count monsters */
		m_cnt++;

		/* Return the index */
		return (i);
	}


	/* Recycle dead monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr;

		/* Acquire monster */
		m_ptr = &m_list[i];

		/* Skip live monsters */
		if (m_ptr->r_idx) continue;

		/* Count monsters */
		m_cnt++;

		/* Use this monster */
		return (i);
	}


	/* Warn the player (except during dungeon creation) */
#ifdef JP
	if (character_dungeon) msg_print("モンスターが多すぎる！");
#else
	if (character_dungeon) msg_print("Too many monsters!");
#endif


	/* Try not to crash */
	return (0);
}




/*
 * Hack -- the "type" of the current "summon specific"
 */
static int summon_specific_type = 0;


/*
 * Hack -- the index of the summoning monster
 */
static int summon_specific_who = -1;


static bool summon_unique_okay = FALSE;


/*:::summon_specific()の召喚可能条件設定の続き type毎に召喚可能かどうか判定する*/
static bool summon_specific_aux(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];
	int okay = FALSE;

	/* Check our requirements */
	switch (summon_specific_type)
	{
		case SUMMON_ANT:
		{
			okay = (r_ptr->d_char == 'a');
			break;
		}

		///mod140109 クモ召喚→虫召喚
		case SUMMON_SPIDER:
		{
			//okay = (r_ptr->d_char == 'S');
			okay = (r_ptr->d_char == 'I' || r_ptr->d_char == 'c');
			break;
		}

		case SUMMON_HOUND:
		{
			okay = ((r_ptr->d_char == 'C') || (r_ptr->d_char == 'Z'));
			break;
		}

		case SUMMON_HYDRA:
		{
			okay = (r_ptr->d_char == 'M');
			break;
		}

		case SUMMON_ANGEL:
		{
			okay = (r_ptr->d_char == 'A' && ((r_ptr->flags3 & RF3_ANG_CHAOS) || (r_ptr->flags3 & RF3_ANG_COSMOS)));
			break;
		}

		case SUMMON_DEMON:
		{
			okay = (r_ptr->flags3 & RF3_DEMON);
			break;
		}

		case SUMMON_DEITY:
		{
			okay = (r_ptr->flags3 & RF3_DEITY);
			break;
		}

		case SUMMON_UNDEAD:
		{
			okay = (r_ptr->flags3 & RF3_UNDEAD);
			break;
		}

		case SUMMON_DRAGON:
		{
			okay = (r_ptr->flags3 & RF3_DRAGON);
			break;
		}

		case SUMMON_HI_UNDEAD:
		{
			okay = ((r_ptr->d_char == 'L') ||
				(r_ptr->d_char == 'V') ||
				(r_ptr->d_char == 'W'));
			break;
		}

		case SUMMON_HI_DRAGON:
		{
			okay = (r_ptr->d_char == 'D');
			break;
		}

		case SUMMON_HI_DEMON:
		{
			okay = (((r_ptr->d_char == 'U') ||
				 (r_ptr->d_char == 'H') ||
				 (r_ptr->d_char == 'B')) &&
				(r_ptr->flags3 & RF3_DEMON)) ? TRUE : FALSE;
			break;
		}

		case SUMMON_AMBERITES:
		{
			okay = (r_ptr->flags3 & (RF3_ANG_AMBER)) ? TRUE : FALSE;
			break;
		}

		case SUMMON_UNIQUE:
		{
			okay = (r_ptr->flags1 & (RF1_UNIQUE)) ? TRUE : FALSE;
			break;
		}

		case SUMMON_BIZARRE1:
		{
			okay = (r_ptr->d_char == 'm');
			break;
		}
		case SUMMON_BIZARRE2:
		{
			okay = (r_ptr->d_char == 'b');
			break;
		}
		case SUMMON_BIZARRE3:
		{
			okay = (r_ptr->d_char == 'Q');
			break;
		}

		case SUMMON_BIZARRE4:
		{
			okay = (r_ptr->d_char == 'v');
			break;
		}

		case SUMMON_BIZARRE5:
		{
			okay = (r_ptr->d_char == '$');
			break;
		}

		case SUMMON_BIZARRE6:
		{
			okay = ((r_ptr->d_char == '!') ||
				 (r_ptr->d_char == '?') ||
				 (r_ptr->d_char == '=') ||
				 (r_ptr->d_char == '$') ||
				 (r_ptr->d_char == '|'));
			break;
		}

		case SUMMON_GOLEM:
		{
			okay = ((r_ptr->d_char == 'g') && (r_idx != MON_ALICE_DOLL));
			break;
		}

		case SUMMON_CYBER:
		{
			okay = ((r_ptr->d_char == 'U') &&
				(r_ptr->flags4 & RF4_ROCKET));
			break;
		}


		case SUMMON_KIN:
		{
			okay = ((r_ptr->d_char == summon_kin_type) && (r_idx != MON_HAGURE));
			break;
		}

		case SUMMON_DAWN:
		{
			okay = (r_idx == MON_DAWN);
			break;
		}

		case SUMMON_ANIMAL:
		{
			okay = (r_ptr->flags3 & (RF3_ANIMAL));
			break;
		}

		case SUMMON_ANIMAL_RANGER:
		{
			okay = ((r_ptr->flags3 & (RF3_ANIMAL)) &&
			       (my_strchr("abcflqrwBCHIJMR", r_ptr->d_char)) &&
			       !(r_ptr->flags3 & (RF3_DRAGON)) &&
			       !(r_ptr->flags3 & (RF3_ANG_CHAOS)) &&
			       !(r_ptr->flags3 & (RF3_UNDEAD)) &&
			       !(r_ptr->flags3 & (RF3_DEMON)) &&
			       !(r_ptr->flags2 & (RF2_MULTIPLY)) &&
			       !(r_ptr->flags4 || r_ptr->flags5 || r_ptr->flags6));
			break;
		}

		case SUMMON_HI_DRAGON_LIVING:
		{
			okay = ((r_ptr->d_char == 'D') && monster_living(r_ptr));
			break;
		}

		case SUMMON_LIVING:
		{
			okay = monster_living(r_ptr);
			break;
		}

		case SUMMON_PHANTOM:
		{
			okay = (r_idx == MON_PHANTOM_B || r_idx == MON_PHANTOM_W);
			break;
		}

		case SUMMON_BLUE_HORROR:
		{
			okay = (r_idx == MON_BLUE_HORROR);
			break;
		}

		case SUMMON_ELEMENTAL:
		{
			okay = (r_ptr->d_char == 'E');
			break;
		}

		case SUMMON_VORTEX:
		{
			okay = (r_ptr->d_char == 'v');
			break;
		}

		case SUMMON_HYBRID:
		{
			okay = (r_ptr->d_char == 'H');
			break;
		}

		case SUMMON_BIRD:
		{
			okay = (r_ptr->d_char == 'B');
			break;
		}

		case SUMMON_KAMIKAZE:
		{
			int i;
			for (i = 0; i < 4; i++)
				if (r_ptr->blow[i].method == RBM_EXPLODE) okay = TRUE;
			break;
		}

		case SUMMON_KAMIKAZE_LIVING:
		{
			int i;

			for (i = 0; i < 4; i++)
				if (r_ptr->blow[i].method == RBM_EXPLODE) okay = TRUE;
			okay = (okay && monster_living(r_ptr));
			break;
		}

		case SUMMON_MANES:
		{
			okay = (r_idx == MON_MANES);
			break;
		}

		case SUMMON_LOUSE:
		{
			okay = (r_idx == MON_LOUSE);
			break;
		}

		case SUMMON_GUARDIANS:
		{
			okay = (r_idx == MON_AZATHOTH || r_idx == MON_MORGOTH);
			break;
		}

		case SUMMON_KNIGHTS:
		{
			okay = ((r_idx == MON_NOV_PALADIN) ||
				(r_idx == MON_NOV_PALADIN_G) ||
				(r_idx == MON_PALADIN) ||
				(r_idx == MON_W_KNIGHT) ||
				(r_idx == MON_ULTRA_PALADIN) ||
				(r_idx == MON_KNI_TEMPLAR));
			break;
		}

		case SUMMON_EAGLES:
		{
			okay = (r_ptr->d_char == 'B' &&
				(r_ptr->flags8 & RF8_WILD_MOUNTAIN) &&
				(r_ptr->flags8 & RF8_WILD_ONLY));
			break;
		}

		case SUMMON_PIRANHAS:
		{
			okay = (r_idx == MON_PIRANHA);
			break;
		}

		case SUMMON_ARMAGE_GOOD:
		{
			okay = (r_ptr->d_char == 'A' && (r_ptr->flags3 & RF3_ANG_COSMOS));
			break;
		}

		case SUMMON_ARMAGE_EVIL:
		{
			okay = ((r_ptr->flags3 & RF3_DEMON) ||
				(r_ptr->d_char == 'A' && (r_ptr->flags3 & RF3_ANG_CHAOS)));
			break;
		}

		case SUMMON_KWAI:
		{
			okay = (r_ptr->flags3 & RF3_KWAI);
			break;
		}


		case SUMMON_DOLLS:
		{
			okay = (r_idx == MON_ALICE_DOLL);
			break;
		}

		case SUMMON_LIB_GOLEM:
		{
			if(dun_level > 49 || one_in_(3))okay = (r_idx == MON_LIB_GOLEM2);
			else okay = (r_idx == MON_LIB_GOLEM);
			break;
		}

		case SUMMON_GREATER_DEMON:
		{
			okay = (r_idx == MON_GREATER_DEMON);
			break;
		}

		case SUMMON_GHOST:
		{
			okay = (r_ptr->d_char == 'G');
			break;
		}
		case SUMMON_DEATH:
		{
			//v1.1.15 ==でなく&だった
			okay = (r_ptr->flags7 & RF7_FROM_HELL);
			break;
		}
		case SUMMON_SKELETON:
		{
			okay = (r_ptr->d_char == 's' || r_idx == MON_SKEL_TYRANNO || r_idx == MON_NIGHTMARE || r_idx == MON_BONEDRAGON);
			break;
		}
		case SUMMON_INSANITY:
		{
			okay = (r_ptr->flags2 & RF2_ELDRITCH_HORROR);
			break;
		}
		case SUMMON_INSANITY2:
		{
			okay = (r_ptr->flags2 & RF2_ELDRITCH_HORROR && r_ptr->flags3 & RF3_DEITY);
			break;
		}

		case SUMMON_INSANITY3:
		{
			okay = (r_ptr->flags2 & RF2_ELDRITCH_HORROR && r_ptr->flags3 & RF3_DEITY || r_idx == MON_FLUTIST);
			break;
		}
		case SUMMON_SNAKE:
		{
			okay = (r_ptr->d_char == 'J');
			break;
		}
		case SUMMON_FAIRY:
		{
			okay = (r_ptr->d_char == 'F');
			break;
		}
		case SUMMON_HUMANOID:
		{
			okay = (r_ptr->d_char == 'p' || r_ptr->d_char == 'P' || r_ptr->d_char == 'h');
			break;
		}
		case SUMMON_PHANTASMAGORIA:
		{
			okay = (is_gen_unique(r_idx));
			break;
		}
		case SUMMON_UNDEAD_DRAGON:
		{
			okay = ( r_ptr->flags3 & RF3_UNDEAD && r_ptr->d_char == 'D' || r_idx == MON_SKEL_TYRANNO || r_idx == MON_S_TYRANNO);
			break;
		}
		case SUMMON_HOLY_ANIMAL:
		{
			okay = ( r_idx == MON_FENGHUANG || r_idx == MON_KIRIN ||  r_idx == MON_OURYU || r_idx == MON_REIKI);
			break;
		}
		case SUMMON_BYAKHEE:
		{
			okay = ( r_idx == MON_BYAKHEE );
			break;
		}
		case SUMMON_ZOMBIE_FAIRY:
		{
			okay = ( r_idx == MON_ZOMBIE_FAIRY );
			break;
		}
		case SUMMON_MUSHROOM:
		{
			okay = ( r_ptr->d_char == ',' && r_idx != MON_TZEENTCH_FLAMER );
			break;
		}
		case SUMMON_MORGOTH_MINION:
		{
			okay = ( r_idx == MON_NAZGUL || r_idx == MON_G_BALROG || r_idx == MON_LUNGORTHIN || r_idx == MON_GOTHMOG
				|| r_idx == MON_THURINGWETHIL || r_idx == MON_SARUMAN || r_idx == MON_MOUTH_OF_SAURON
				|| r_idx == MON_DRAUGLUIN || r_idx == MON_SAURON || r_idx == MON_ANCALAGON || r_idx == MON_ANGMAR
				|| r_idx == MON_KHAMUL || r_idx == MON_HOARMURATH || r_idx == MON_CARCHAROTH);
			break;
		}
		case SUMMON_DESIRE:
		{
			okay = (r_idx == S_SHINREI_B || r_idx == S_SHINREI_P || r_idx == S_SHINREI_G || r_idx == S_SHINREI_W);
			break;
		}
		case SUMMON_MAGICSIGN:
		{
			okay = (r_idx == MON_SIGN_R || r_idx == MON_SIGN_B || r_idx == MON_SIGN_W || r_idx == MON_SIGN_L || r_idx == MON_SIGN_S);
			break;
		}
		case SUMMON_MINI_SUIKA:
		{
			okay = (r_idx == MON_MINI_SUIKA);
			break;
		}
		case SUMMON_TANUKI:
		{
			okay = (r_idx == MON_TANUKI || r_idx == MON_O_TANUKI);
			break;
		}
		case SUMMON_GYOKUTO:
		{
			okay = (r_idx == MON_EAGLE_RABBIT || r_idx == MON_GYOKUTO);
			break;
		}
		case SUMMON_RAT:
		{
			okay = (r_idx == MON_YOUKAI_MOUSE || r_idx == MON_WHITEMOUSE || r_idx == MON_WHITE_RAT
				|| r_idx == MON_RAT_THING || r_idx == MON_GIANT_GREY_RAT || r_idx == MON_WERERAT);
			break;
		}
		case SUMMON_REMY_BAT:
		{
			okay = (r_idx == MON_V_BAT);
			break;
		}
		case SUMMON_ORCS:
		{
			okay = (r_ptr->d_char == 'o' && (r_ptr->flags1 & RF1_ESCORT));
			break;

		}
		case SUMMON_ONE_ORC:
		{
			okay = (r_ptr->d_char == 'o');
			break;

		}

		case SUMMON_LUNARIAN:
		{
			okay = (r_ptr->flags3 & RF3_GEN_MOON);
			break;

		}
		case SUMMON_TOYOHIME_TROOPS:
		{
			okay = (!(r_ptr->flags3 & (RF3_GEN_MASK | RF3_ANG_CHAOS)));
			break;
		}
		case SUMMON_TSUKUMO:
		{
			okay = (r_idx == MON_TSUKUMO_1 || r_idx == MON_TSUKUMO_2 && one_in_(3));
			break;
		}
		case SUMMON_GENSOUKYOU:
		{
			okay = ((r_ptr->flags3 & RF3_GEN_MASK) && !(r_ptr->flags1 & RF1_QUESTOR) && !(r_ptr->flags3 & RF3_WANTED));
			break;
		}
		case SUMMON_HANIWA:
		{
			okay = (r_ptr->flags3 & (RF3_HANIWA));
			break;
		}
		case SUMMON_AQUATIC:
		{
			okay = (r_ptr->flags7 & (RF7_AQUATIC));
			break;

		}

		case SUMMON_ONLY_GOLD:
		{
			okay = (r_ptr->flags1 & (RF1_ONLY_GOLD));
			break;

		}

		//龍が呼ぶ天狗配下　天狗ユニークと大天狗を除く一般天狗
		case SUMMON_TENGU_MINION:
		{
			okay = (r_ptr->d_char == 't' && r_idx != MON_G_TENGU && !(r_ptr->flags1 & RF1_UNIQUE));
			break;
		}


	}

	/* Result */
	/* Since okay is int, "return (okay);" is not correct. */
	return (bool)(okay ? TRUE : FALSE);
}

/*:::カメレオンの変身処理中には0以外が入ってるらしい*/
static int chameleon_change_m_idx = 0;


/*
 * Some dungeon types restrict the possible monsters.
 * Return TRUE is the monster is OK and FALSE otherwise
 */
///system モンスターフラグ
/*:::ダンジョン毎に特定のモンスターの出現を抑止する*/
static bool restrict_monster_to_dungeon(int r_idx)
{
	dungeon_info_type *d_ptr = &d_info[dungeon_type];
	monster_race *r_ptr = &r_info[r_idx];
	byte a;

	if (d_ptr->flags1 & DF1_CHAMELEON)
	{
		if (chameleon_change_m_idx) return TRUE;
	}
	/*:::反魔法の洞窟では、魔法を使うモンスターは魔法以外のスキルを持ってないと出られない*/
	if (d_ptr->flags1 & DF1_NO_MAGIC)
	{
		if (
		    r_ptr->freq_spell &&
		    !(r_ptr->flags4 & RF4_NOMAGIC_MASK) &&
		    !(r_ptr->flags5 & RF5_NOMAGIC_MASK) &&
		    !(r_ptr->flags6 & RF6_NOMAGIC_MASK))
			return FALSE;
		///mod140629 カメレオンを妖怪狐として再実装　RF9はどうせ全部魔法なので無視
		if (r_idx != MON_CHAMELEON &&
		    r_ptr->freq_spell &&
		    !(r_ptr->flags4 & RF4_NOMAGIC_MASK) &&
		    !(r_ptr->flags5 & RF5_NOMAGIC_MASK) &&
		    !(r_ptr->flags6 & RF6_NOMAGIC_MASK))
			return FALSE;
	}
	/*:::反攻撃の洞窟では、魔法以外のスキルしか持たないモンスターは出られない*/
	if (d_ptr->flags1 & DF1_NO_MELEE)
	{
		if (r_idx == MON_CHAMELEON) return TRUE;
		if (!(r_ptr->flags4 & (RF4_BOLT_MASK | RF4_BEAM_MASK | RF4_BALL_MASK)) &&
		    !(r_ptr->flags5 & (RF5_BOLT_MASK | RF5_BEAM_MASK | RF5_BALL_MASK | RF5_CAUSE_1 | RF5_CAUSE_2 | RF5_CAUSE_3 | RF5_CAUSE_4 | RF5_MIND_BLAST | RF5_BRAIN_SMASH)) &&
		    !(r_ptr->flags6 & (RF6_BOLT_MASK | RF6_BEAM_MASK | RF6_BALL_MASK)) &&
		    !(r_ptr->flags9 & (RF9_BOLT_MASK | RF9_BEAM_MASK | RF9_BALL_MASK | RF9_PUNISH_1 | RF9_PUNISH_2 | RF9_PUNISH_3 | RF9_PUNISH_4))
			)
			return FALSE;
	}
	/*:::初心者ダンジョン（イーク？）では階以上のモンスターは出ない*/
	/*:::可変パラメータユニーク、ランダムユニークも出なくした*/
	//輝夜(主人)は除外
	if (d_ptr->flags1 & DF1_BEGINNER && r_idx != MON_MASTER_KAGUYA)
	{
		if (r_ptr->level > dun_level)	return FALSE;
		if ((r_ptr->flags7 & RF7_VARIABLE) && (r_ptr->flags7 & RF7_UNIQUE2))
		{
			if(cheat_hear) msg_print(_("初心者ダンジョンのため特殊ユニークが生成抑止された",
                                        "Special uniques were not generated in beginner dungeon"));
			return FALSE;
		}

	}

	if (d_ptr->special_div >= 64) return TRUE;
	if (summon_specific_type && !(d_ptr->flags1 & DF1_CHAMELEON)) return TRUE;

	/*:::この先、ダンジョンフラグとモンスターフラグを比較して一致したモンスターしか出さない処理*/
	switch (d_ptr->mode)
	{
	case DUNGEON_MODE_AND:
		if (d_ptr->mflags1)
		{
			if ((d_ptr->mflags1 & r_ptr->flags1) != d_ptr->mflags1)
				return FALSE;
		}
		if (d_ptr->mflags2)
		{
			if ((d_ptr->mflags2 & r_ptr->flags2) != d_ptr->mflags2)
				return FALSE;
		}
		if (d_ptr->mflags3)
		{
			if ((d_ptr->mflags3 & r_ptr->flags3) != d_ptr->mflags3)
				return FALSE;
		}
		if (d_ptr->mflags4)
		{
			if ((d_ptr->mflags4 & r_ptr->flags4) != d_ptr->mflags4)
				return FALSE;
		}
		if (d_ptr->mflags5)
		{
			if ((d_ptr->mflags5 & r_ptr->flags5) != d_ptr->mflags5)
				return FALSE;
		}
		if (d_ptr->mflags6)
		{
			if ((d_ptr->mflags6 & r_ptr->flags6) != d_ptr->mflags6)
				return FALSE;
		}
		if (d_ptr->mflags7)
		{
			if ((d_ptr->mflags7 & r_ptr->flags7) != d_ptr->mflags7)
				return FALSE;
		}
		if (d_ptr->mflags8)
		{
			if ((d_ptr->mflags8 & r_ptr->flags8) != d_ptr->mflags8)
				return FALSE;
		}
		if (d_ptr->mflags9)
		{
			if ((d_ptr->mflags9 & r_ptr->flags9) != d_ptr->mflags9)
				return FALSE;
		}
		if (d_ptr->mflagsr)
		{
			if ((d_ptr->mflagsr & r_ptr->flagsr) != d_ptr->mflagsr)
				return FALSE;
		}
		for (a = 0; a < DUNGEON_R_CHAR_MAX; a++)
			if (d_ptr->r_char[a] && (d_ptr->r_char[a] != r_ptr->d_char)) return FALSE;

		return TRUE;

	case DUNGEON_MODE_NAND:
		if (d_ptr->mflags1)
		{
			if ((d_ptr->mflags1 & r_ptr->flags1) != d_ptr->mflags1)
				return TRUE;
		}
		if (d_ptr->mflags2)
		{
			if ((d_ptr->mflags2 & r_ptr->flags2) != d_ptr->mflags2)
				return TRUE;
		}
		if (d_ptr->mflags3)
		{
			if ((d_ptr->mflags3 & r_ptr->flags3) != d_ptr->mflags3)
				return TRUE;
		}
		if (d_ptr->mflags4)
		{
			if ((d_ptr->mflags4 & r_ptr->flags4) != d_ptr->mflags4)
				return TRUE;
		}
		if (d_ptr->mflags5)
		{
			if ((d_ptr->mflags5 & r_ptr->flags5) != d_ptr->mflags5)
				return TRUE;
		}
		if (d_ptr->mflags6)
		{
			if ((d_ptr->mflags6 & r_ptr->flags6) != d_ptr->mflags6)
				return TRUE;
		}
		if (d_ptr->mflags7)
		{
			if ((d_ptr->mflags7 & r_ptr->flags7) != d_ptr->mflags7)
				return TRUE;
		}
		if (d_ptr->mflags8)
		{
			if ((d_ptr->mflags8 & r_ptr->flags8) != d_ptr->mflags8)
				return TRUE;
		}
		if (d_ptr->mflags9)
		{
			if ((d_ptr->mflags9 & r_ptr->flags9) != d_ptr->mflags9)
				return TRUE;
		}
		if (d_ptr->mflagsr)
		{
			if ((d_ptr->mflagsr & r_ptr->flagsr) != d_ptr->mflagsr)
				return TRUE;
		}
		for (a = 0; a < 5; a++)
			if (d_ptr->r_char[a] && (d_ptr->r_char[a] != r_ptr->d_char)) return TRUE;

		return FALSE;

	case DUNGEON_MODE_OR:
		if (r_ptr->flags1 & d_ptr->mflags1) return TRUE;
		if (r_ptr->flags2 & d_ptr->mflags2) return TRUE;
		if (r_ptr->flags3 & d_ptr->mflags3) return TRUE;
		if (r_ptr->flags4 & d_ptr->mflags4) return TRUE;
		if (r_ptr->flags5 & d_ptr->mflags5) return TRUE;
		if (r_ptr->flags6 & d_ptr->mflags6) return TRUE;
		if (r_ptr->flags7 & d_ptr->mflags7) return TRUE;
		if (r_ptr->flags8 & d_ptr->mflags8) return TRUE;
		if (r_ptr->flags9 & d_ptr->mflags9) return TRUE;
		if (r_ptr->flagsr & d_ptr->mflagsr) return TRUE;
		for (a = 0; a < DUNGEON_R_CHAR_MAX; a++)
			if (d_ptr->r_char[a] == r_ptr->d_char) return TRUE;

		return FALSE;

	case DUNGEON_MODE_NOR:
		if (r_ptr->flags1 & d_ptr->mflags1) return FALSE;
		if (r_ptr->flags2 & d_ptr->mflags2) return FALSE;
		if (r_ptr->flags3 & d_ptr->mflags3) return FALSE;
		if (r_ptr->flags4 & d_ptr->mflags4) return FALSE;
		if (r_ptr->flags5 & d_ptr->mflags5) return FALSE;
		if (r_ptr->flags6 & d_ptr->mflags6) return FALSE;
		if (r_ptr->flags7 & d_ptr->mflags7) return FALSE;
		if (r_ptr->flags8 & d_ptr->mflags8) return FALSE;
		if (r_ptr->flags9 & d_ptr->mflags9) return FALSE;
		if (r_ptr->flagsr & d_ptr->mflagsr) return FALSE;
		for (a = 0; a < DUNGEON_R_CHAR_MAX; a++)
			if (d_ptr->r_char[a] == r_ptr->d_char) return FALSE;

		return TRUE;
	}

	return TRUE;
}

/*
 * Apply a "monster restriction function" to the "monster allocation table"
 */
/*:::alloc_race_tableに出現可能なモンスターのリストを作る？*/
/*:::引数は関数へのポインタ*/
errr get_mon_num_prep(monster_hook_type monster_hook,
					  monster_hook_type monster_hook2)
{
	int i;

	/* Todo: Check the hooks for non-changes */

	/* Set the new hooks */
	/*:::引数の関数アドレスを代入*/
	get_mon_num_hook = monster_hook;
	get_mon_num2_hook = monster_hook2;

	/* Scan the allocation table */
	/*:::alloc_race_size = max_r_idx*/
	for (i = 0; i < alloc_race_size; i++)
	{
		monster_race	*r_ptr;

		/* Get the entry */
		alloc_entry *entry = &alloc_race_table[i];

		entry->prob2 = 0;
		/*:::entry->indexは、init_alloc()で既に数値の代入が済んでいる*/
		r_ptr = &r_info[entry->index];



		/*:::*/
		/* Skip monsters which don't pass the restriction */
		if ((get_mon_num_hook && !((*get_mon_num_hook)(entry->index))) ||
		    (get_mon_num2_hook && !((*get_mon_num2_hook)(entry->index))))
			continue;

		if (!p_ptr->inside_battle && !p_ptr->inside_arena && !chameleon_change_m_idx &&
		    summon_specific_type != SUMMON_GUARDIANS)
		{
			/* Hack -- don't create questors */
			if (r_ptr->flags1 & RF1_QUESTOR)
				continue;

			if (r_ptr->flags7 & RF7_GUARDIAN)
				continue;

			/* Depth Monsters never appear out of depth */
			///mod141124 LUNATIC難度で制限解除
			if ((r_ptr->flags1 & (RF1_FORCE_DEPTH)) &&
			    (r_ptr->level > dun_level) && !ironman_nightmare )
				continue;

		}
		/* Accept this monster */
		entry->prob2 = entry->prob1;

		if (dun_level && (!p_ptr->inside_quest || is_fixed_quest_idx(p_ptr->inside_quest)) && !restrict_monster_to_dungeon(entry->index) && !p_ptr->inside_battle)
		{
			int hoge = entry->prob2 * d_info[dungeon_type].special_div;
			entry->prob2 = hoge / 64;
			if (randint0(64) < (hoge & 0x3f)) entry->prob2++;
		}


		///mod140301 生息地実装
		/// この辺りでextraが0のはずのモンスターがときどき3になってる。なぜ？どっかでうっかり代入した？
		/// ↑extraを3にしているモンスターだった。alloc_tableがレベルで整列されるからiはm_idxでないということを忘れていた。
		if((r_ptr->extra != 0) && (r_ptr->extra != dungeon_type))
		{
			if(r_ptr->flags8 & RF8_STAY_AT_HOME3) entry->prob2 = 0;
			else if(r_ptr->flags8 & RF8_STAY_AT_HOME2) entry->prob2 /= 10;
			else if(r_ptr->flags8 & RF8_STAY_AT_HOME1) entry->prob2 /= 4;
			else entry->prob2 /= 2;
			//msg_format("Restricted (Ex:%d prob:%d name:%s)",r_ptr->extra,entry->prob2,r_name + r_ptr->name );
		}

	}

	/* Success */
	return (0);
}

/*:::よく読んでないが平方根の近似値？*/
static int mysqrt(int n)
{
	int tmp = n>>1;
	int tasu = 10;
	int kaeriti = 1;

	if (!tmp)
	{
		if (n) return 1;
		else return 0;
	}

	while(tmp)
	{
		if ((n/tmp) < tmp)
		{
			tmp >>= 1;
		}
		else break;
	}
	kaeriti = tmp;
	while(tasu)
	{
		if ((n/tmp) < tmp)
		{
			tasu--;
			tmp = kaeriti;
		}
		else
		{
			kaeriti = tmp;
			tmp += tasu;
		}
	}
	return kaeriti;
}

/*
 * Choose a monster race that seems "appropriate" to the given level
 *
 * This function uses the "prob2" field of the "monster allocation table",
 * and various local information, to calculate the "prob3" field of the
 * same table, which is then used to choose an "appropriate" monster, in
 * a relatively efficient manner.
 *
 * Note that "town" monsters will *only* be created in the town, and
 * "normal" monsters will *never* be created in the town, unless the
 * "level" is "modified", for example, by polymorph or summoning.
 *
 * There is a small chance (1/50) of "boosting" the given depth by
 * a small amount (up to four levels), except in the town.
 *
 * It is (slightly) more likely to acquire a monster of the given level
 * than one of a lower level.  This is done by choosing several monsters
 * appropriate to the given level and keeping the "hardest" one.
 *
 * Note that if no monsters are "appropriate", then this function will
 * fail, and return zero, but this should *almost* never happen.
 */
/*:::ダンジョンレベルに応じてモンスターを一体選定し、そのIDを返す。*/
/*:::確率で生成レベルがブーストされる。ターン数が多いと上がりやすい　最大1/3で+25？*/
/*:::低いレベルのモンスターはやや出にくい。*/
/*:::街では特別処理*/

s16b get_mon_num(int level)
{
	int			i, j, p;
	int			r_idx;
	long		value, total;
	monster_race	*r_ptr;
	/*:::get_mon_num_prep()によって調整を受けた出現可能モンスターリスト*/
	alloc_entry		*table = alloc_race_table;

	int pls_kakuritu, pls_level;
	int hoge = mysqrt(level*10000L);

	int shion_card_num = 0;

	/*:::levelは128以上にはならない*/
	if (level > MAX_DEPTH - 1) level = MAX_DEPTH - 1;

	/*:::あまりゲームターンが長いとモンスターのレベルがブーストされやすい？*/
	pls_kakuritu = MAX(NASTY_MON_MAX, NASTY_MON_BASE - ((dungeon_turn / (TURNS_PER_TICK * 2500L) - hoge / 10)));
	pls_level    = MIN(NASTY_MON_PLUS_MAX, 3 + dungeon_turn / (TURNS_PER_TICK * 20000L) - hoge / 40 + MIN(5, level / 10)) ;


	/*:::Hack- 無縁塚ではレベルのぶっ飛んだ敵が出やすい*/
	///mod141231 不運状態の時も同様になる
	///mod150308 ちょっと出現率下げた
	if((dungeon_type == DUNGEON_MUEN ||  p_ptr->today_mon == FORTUNETELLER_BADLUCK) && one_in_(13))
	{
		pls_kakuritu  = NASTY_MON_MAX;
		pls_level = NASTY_MON_PLUS_MAX;
	}

	/*:::迷宮用補正らしい*/
	if (d_info[dungeon_type].flags1 & DF1_MAZE)
	{
		pls_kakuritu = MIN(pls_kakuritu / 2, pls_kakuritu - 10);
		if (pls_kakuritu < 2) pls_kakuritu = 2;
		pls_level += 2;
		level += 3;
	}

	//v2.0.1 アビリティカード「煤けた団扇」
	shion_card_num = count_ability_card(ABL_CARD_SHION);

	if (shion_card_num)
	{
		int mult = calc_ability_card_mod_param(ABL_CARD_SHION, shion_card_num);

		//ランダムモンスターレベルブースト確率低下
		pls_kakuritu = pls_kakuritu * mult / 100;

		//フロアよりハイレベルでの生成を確率でフロアレベルまで下げる
		if (level > dun_level && randint1(mult) > 100)
		{
			level = dun_level;
		}

	}

	/* Boost the level */

	if (!p_ptr->inside_battle && !(d_info[dungeon_type].flags1 & DF1_BEGINNER))
	{
		/* Nightmare mode allows more out-of depth monsters */
		/*:::悪夢モードのとき、イーク窟以外でモンスター生成レベルがぶっ飛ぶ*/
		if (ironman_nightmare && !randint0(pls_kakuritu))
		{
			/* What a bizarre calculation */
			level = 1 + (level * MAX_DEPTH / randint1(MAX_DEPTH));
		}
		else
		{
			/*:::悪夢モード以外のとき、確率でレベルブースト*/
			/*:::ターン長い→pls_kakuritu低くなる→randint0が0になりやすい→レベルブーストしやすい*/
			/* Occasional "nasty" monster */
			if (!randint0(pls_kakuritu))
			{
				/* Pick a level bonus */
				level += pls_level;
			}
		}
	}
/*:::level確定*/


	/* Reset total */
	total = 0L;
	/* Process probabilities */
	/*:::alloc_race_table[]をループ get_mon_num_prep()でtable.prob2に0（非出現）～100の値が格納済み*/
	/*:::alloc_race_size:alloc_race_tableのサイズ max_r_idxに等しい*/
	for (i = 0; i < alloc_race_size; i++)
	{
		/* Monsters are sorted by depth */
		if (table[i].level > level) break;

		/* Default */
		table[i].prob3 = 0;

		/* Access the "r_idx" of the chosen monster */
		r_idx = table[i].index;

		/* Access the actual race */
		r_ptr = &r_info[r_idx];

		/*:::闘技場でもカメレオン変身中でもない場合*/
		//v1.1.51 新アリーナ追加
		if (!p_ptr->inside_battle && !p_ptr->inside_arena && !chameleon_change_m_idx)
		{
			/* Hack -- "unique" monsters must be "unique" */
			/*:::ユニークと準ユニークは特定数しか出現しない*/
			if (((r_ptr->flags1 & (RF1_UNIQUE)) ||
			     (r_ptr->flags7 & (RF7_NAZGUL))) &&
			    (r_ptr->cur_num >= r_ptr->max_num))
			{
				continue;
			}

			if ((r_ptr->flags7 & (RF7_UNIQUE2)) &&
			    (r_ptr->cur_num >= 1))
			{
				continue;
			}
			///mons バーノールルパート特殊処理
			if (r_idx == MON_BANORLUPART)
			{
				if (r_info[MON_BANOR].cur_num > 0) continue;
				if (r_info[MON_LUPART].cur_num > 0) continue;
			}


		}


		/* Accept */
		/*:::get_mon_num_prep()でtable.prob2に0（非出現）～100の値が格納済みのはず*/
		table[i].prob3 = table[i].prob2;

		/* Total */
		/*:::最終的にレベル1～levelまでの全ての有効なモンスターのtable[].prob3の値が合計される*/
		total += table[i].prob3;
	}
	/* No legal monsters */
	if (total <= 0) return (0);

	/* Pick a monster */
	value = randint0(total);

	/* Find the monster */
	/*:::ここでprob2に重み付けされた確率でモンスターインデックスが選ばれる*/
	for (i = 0; i < alloc_race_size; i++)
	{
		/* Found the entry */
		if (value < table[i].prob3) break;

		/* Decrement */
		value = value - table[i].prob3;
	}

	///mod140625 可変パラメータモンスター関連
	///mod150614 なんかランダムユニークとかが出ないと思ったら設定を間違えてた。修正。
	/*:::Mega Hack - 霊夢や魔理沙などの可変パラメータモンスターはalloc_race_tableの時点ではレベル1のため、再抽選で消えないようにここで終了する*/
	//if(r_info[r_idx].flags7 & RF7_VARIABLE) return (table[i].index);
	if(r_info[table[i].index].flags7 & RF7_VARIABLE)
	{
		return (table[i].index);
	}

	/*:::確率で最高あと2回再抽選を行い、一番高いレベルのモンスターを選定する。これにより出現モンスターがlevel近辺にやや偏る*/
	/* Power boost */
	p = randint0(100);

	/* Try for a "harder" monster once (50%) or twice (10%) */
	if (p < 60)
	{
		/* Save old */
		j = i;

		/* Pick a monster */
		value = randint0(total);

		/* Find the monster */
		for (i = 0; i < alloc_race_size; i++)
		{
			/* Found the entry */
			if (value < table[i].prob3) break;

			/* Decrement */
			value = value - table[i].prob3;
		}

		/* Keep the "best" one */
		if (table[i].level < table[j].level) i = j;
	}

	/* Try for a "harder" monster twice (10%) */
	if (p < 10)
	{
		/* Save old */
		j = i;

		/* Pick a monster */
		value = randint0(total);

		/* Find the monster */
		for (i = 0; i < alloc_race_size; i++)
		{
			/* Found the entry */
			if (value < table[i].prob3) break;

			/* Decrement */
			value = value - table[i].prob3;
		}

		/* Keep the "best" one */
		if (table[i].level < table[j].level) i = j;
	}

	/* Result */
	return (table[i].index);
}





/*
 * Build a string describing a monster in some way.
 *
 * We can correctly describe monsters based on their visibility.
 * We can force all monsters to be treated as visible or invisible.
 * We can build nominatives, objectives, possessives, or reflexives.
 * We can selectively pronominalize hidden, visible, or all monsters.
 * We can use definite or indefinite descriptions for hidden monsters.
 * We can use definite or indefinite descriptions for visible monsters.
 *
 * Pronominalization involves the gender whenever possible and allowed,
 * so that by cleverly requesting pronominalization / visibility, you
 * can get messages like "You hit someone.  She screams in agony!".
 *
 * Reflexives are acquired by requesting Objective plus Possessive.
 *
 * If no m_ptr arg is given (?), the monster is assumed to be hidden,
 * unless the "Assume Visible" mode is requested.
 *
 * If no r_ptr arg is given, it is extracted from m_ptr and r_info
 * If neither m_ptr nor r_ptr is given, the monster is assumed to
 * be neuter, singular, and hidden (unless "Assume Visible" is set),
 * in which case you may be in trouble... :-)
 *
 * I am assuming that no monster name is more than 70 characters long,
 * so that "char desc[80];" is sufficiently large for any result.
 *
 * Mode Flags:
 *  MD_OBJECTIVE      --> Objective (or Reflexive)
 *  MD_POSSESSIVE     --> Possessive (or Reflexive)
 *  MD_INDEF_HIDDEN   --> Use indefinites for hidden monsters ("something")
 *  MD_INDEF_VISIBLE  --> Use indefinites for visible monsters ("a kobold")
 *  MD_PRON_HIDDEN    --> Pronominalize hidden monsters
 *  MD_PRON_VISIBLE   --> Pronominalize visible monsters
 *  MD_ASSUME_HIDDEN  --> Assume the monster is hidden
 *  MD_ASSUME_VISIBLE --> Assume the monster is visible
 *  MD_TRUE_NAME      --> Chameleon's true name
 *  MD_IGNORE_HALLU   --> Ignore hallucination, and penetrate shape change
 *
 * Useful Modes:
 *  0x00 --> Full nominative name ("the kobold") or "it"
 *  MD_INDEF_HIDDEN --> Full nominative name ("the kobold") or "something"
 *  MD_ASSUME_VISIBLE --> Genocide resistance name ("the kobold")
 *  MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE --> Killing name ("a kobold")
 *  MD_PRON_VISIBLE | MD_POSSESSIVE
 *    --> Possessive, genderized if visable ("his") or "its"
 *  MD_PRON_VISIBLE | MD_POSSESSIVE | MD_OBJECTIVE
 *    --> Reflexive, genderized if visable ("himself") or "itself"
 */
/*:::monster_typeや可視不可視などの情報からモンスターの記述を決めるらしい*/
///mon モンスター記述関連
void monster_desc(char *desc, monster_type *m_ptr, int mode)
{
	cptr            res;
	monster_race    *r_ptr;

	cptr            name;
	char            buf[128];
	char            buf2[255];
	char            silly_name[1024];
	bool            seen, pron;
	bool            named = FALSE;

	r_ptr = &r_info[m_ptr->ap_r_idx];

	/* Mode of MD_TRUE_NAME will reveal Chameleon's true name */
	if (mode & MD_TRUE_NAME) name = (r_name + real_r_ptr(m_ptr)->name);
	else name = (r_name + r_ptr->name);

	/*:::Hack - 一部可変パラメータのモンスターはr_infoの名前を読み込んだ後別の名前で上書きする*/
	if(m_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && m_ptr->r_idx <= MON_TSUKUMO_WEAPON5 )
	{
		object_desc(buf2,&o_list[m_ptr->hold_o_idx],OD_NAME_ONLY);
		name = buf2;
	}
	else if(IS_RANDOM_UNIQUE_IDX(m_ptr->r_idx))
	{
		name = random_unique_name[m_ptr->r_idx - MON_RANDOM_UNIQUE_1];
	}
	/* Are we hallucinating? (Idea from Nethack...) */
	if (p_ptr->image && !(mode & MD_IGNORE_HALLU))
	{

		if (one_in_(2))
		{
#ifdef JP
			if (!get_rnd_line("silly_j.txt", m_ptr->r_idx, silly_name))
#else
			if (!get_rnd_line("silly.txt", m_ptr->r_idx, silly_name))
#endif

				named = TRUE;
		}

		if (!named)
		{
			monster_race *hallu_race;

			do
			{
				hallu_race = &r_info[randint1(max_r_idx - 1)];
			} ///mod141102 幻覚時、(ランダムユニーク1)みたいな名前が出ないようにする
			while (!hallu_race->name || (hallu_race->flags1 & RF1_UNIQUE) || (hallu_race->flags7 & RF7_VARIABLE));


			strcpy(silly_name, (r_name + hallu_race->name));
		}

		/* Better not strcpy it, or we could corrupt r_info... */
		name = silly_name;
	}

	/* Can we "see" it (exists + forced, or visible + not unforced) */
	seen = (m_ptr && ((mode & MD_ASSUME_VISIBLE) || (!(mode & MD_ASSUME_HIDDEN) && m_ptr->ml)));

	/*:::特殊処理*/
	if(m_ptr->ap_r_idx == MON_KOISHI && p_ptr->pclass != CLASS_KOISHI) seen = FALSE;

	//ぬえの正体不明の種
	if(m_ptr->mflag & MFLAG_NUE_UNKNOWN ) seen = FALSE;

	/* Sexed Pronouns (seen and allowed, or unseen and allowed) */
	pron = (m_ptr && ((seen && (mode & MD_PRON_VISIBLE)) || (!seen && (mode & MD_PRON_HIDDEN))));


	/* First, try using pronouns, or describing hidden monsters */
	if (!seen || pron)
	{
		/* an encoding of the monster "sex" */
		int kind = 0x00;

		/* Extract the gender (if applicable) */
		if (r_ptr->flags1 & (RF1_FEMALE)) kind = 0x20;
		else if (r_ptr->flags1 & (RF1_MALE)) kind = 0x10;

		/* Ignore the gender (if desired) */
		if (!m_ptr || !pron) kind = 0x00;

		/* Assume simple result */
#ifdef JP
		res = "何か";
#else
		res = "it";
#endif


		/* Brute force: split on the possibilities */
		switch (kind + (mode & (MD_INDEF_HIDDEN | MD_POSSESSIVE | MD_OBJECTIVE)))
		{
			/* Neuter, or unknown */
#ifdef JP
			case 0x00:                                                    res = "何か"; break;
			case 0x00 + (MD_OBJECTIVE):                                   res = "何か"; break;
			case 0x00 + (MD_POSSESSIVE):                                  res = "何かの"; break;
			case 0x00 + (MD_POSSESSIVE | MD_OBJECTIVE):                   res = "何か自身"; break;
			case 0x00 + (MD_INDEF_HIDDEN):                                res = "何か"; break;
			case 0x00 + (MD_INDEF_HIDDEN | MD_OBJECTIVE):                 res = "何か"; break;
			case 0x00 + (MD_INDEF_HIDDEN | MD_POSSESSIVE):                res = "何か"; break;
			case 0x00 + (MD_INDEF_HIDDEN | MD_POSSESSIVE | MD_OBJECTIVE): res = "それ自身"; break;
#else
			case 0x00:                                                    res = "it"; break;
			case 0x00 + (MD_OBJECTIVE):                                   res = "it"; break;
			case 0x00 + (MD_POSSESSIVE):                                  res = "its"; break;
			case 0x00 + (MD_POSSESSIVE | MD_OBJECTIVE):                   res = "itself"; break;
			case 0x00 + (MD_INDEF_HIDDEN):                                res = "something"; break;
			case 0x00 + (MD_INDEF_HIDDEN | MD_OBJECTIVE):                 res = "something"; break;
			case 0x00 + (MD_INDEF_HIDDEN | MD_POSSESSIVE):                res = "something's"; break;
			case 0x00 + (MD_INDEF_HIDDEN | MD_POSSESSIVE | MD_OBJECTIVE): res = "itself"; break;
#endif


			/* Male (assume human if vague) */
#ifdef JP
			case 0x10:                                                    res = "彼"; break;
			case 0x10 + (MD_OBJECTIVE):                                   res = "彼"; break;
			case 0x10 + (MD_POSSESSIVE):                                  res = "彼の"; break;
			case 0x10 + (MD_POSSESSIVE | MD_OBJECTIVE):                   res = "彼自身"; break;
			case 0x10 + (MD_INDEF_HIDDEN):                                res = "誰か"; break;
			case 0x10 + (MD_INDEF_HIDDEN | MD_OBJECTIVE):                 res = "誰か"; break;
			case 0x10 + (MD_INDEF_HIDDEN | MD_POSSESSIVE):                res = "誰かの"; break;
			case 0x10 + (MD_INDEF_HIDDEN | MD_POSSESSIVE | MD_OBJECTIVE): res = "彼自身"; break;
#else
			case 0x10:                                                    res = "he"; break;
			case 0x10 + (MD_OBJECTIVE):                                   res = "him"; break;
			case 0x10 + (MD_POSSESSIVE):                                  res = "his"; break;
			case 0x10 + (MD_POSSESSIVE | MD_OBJECTIVE):                   res = "himself"; break;
			case 0x10 + (MD_INDEF_HIDDEN):                                res = "someone"; break;
			case 0x10 + (MD_INDEF_HIDDEN | MD_OBJECTIVE):                 res = "someone"; break;
			case 0x10 + (MD_INDEF_HIDDEN | MD_POSSESSIVE):                res = "someone's"; break;
			case 0x10 + (MD_INDEF_HIDDEN | MD_POSSESSIVE | MD_OBJECTIVE): res = "himself"; break;
#endif


			/* Female (assume human if vague) */
#ifdef JP
			case 0x20:                                                    res = "彼女"; break;
			case 0x20 + (MD_OBJECTIVE):                                   res = "彼女"; break;
			case 0x20 + (MD_POSSESSIVE):                                  res = "彼女の"; break;
			case 0x20 + (MD_POSSESSIVE | MD_OBJECTIVE):                   res = "彼女自身"; break;
			case 0x20 + (MD_INDEF_HIDDEN):                                res = "誰か"; break;
			case 0x20 + (MD_INDEF_HIDDEN | MD_OBJECTIVE):                 res = "誰か"; break;
			case 0x20 + (MD_INDEF_HIDDEN | MD_POSSESSIVE):                res = "誰かの"; break;
			case 0x20 + (MD_INDEF_HIDDEN | MD_POSSESSIVE | MD_OBJECTIVE): res = "彼女自身"; break;
#else
			case 0x20:                                                    res = "she"; break;
			case 0x20 + (MD_OBJECTIVE):                                   res = "her"; break;
			case 0x20 + (MD_POSSESSIVE):                                  res = "her"; break;
			case 0x20 + (MD_POSSESSIVE | MD_OBJECTIVE):                   res = "herself"; break;
			case 0x20 + (MD_INDEF_HIDDEN):                                res = "someone"; break;
			case 0x20 + (MD_INDEF_HIDDEN | MD_OBJECTIVE):                 res = "someone"; break;
			case 0x20 + (MD_INDEF_HIDDEN | MD_POSSESSIVE):                res = "someone's"; break;
			case 0x20 + (MD_INDEF_HIDDEN | MD_POSSESSIVE | MD_OBJECTIVE): res = "herself"; break;
#endif
		}

		/* Copy the result */
		(void)strcpy(desc, res);
	}


	/* Handle visible monsters, "reflexive" request */
	else if ((mode & (MD_POSSESSIVE | MD_OBJECTIVE)) == (MD_POSSESSIVE | MD_OBJECTIVE))
	{
		/* The monster is visible, so use its gender */
#ifdef JP
		if (r_ptr->flags1 & (RF1_FEMALE)) strcpy(desc, "彼女自身");
		else if (r_ptr->flags1 & (RF1_MALE)) strcpy(desc, "彼自身");
		else strcpy(desc, "それ自身");
#else
		if (r_ptr->flags1 & RF1_FEMALE) strcpy(desc, "herself");
		else if (r_ptr->flags1 & RF1_MALE) strcpy(desc, "himself");
		else strcpy(desc, "itself");
#endif
	}


	/* Handle all other visible monster requests */
	else
	{
		/* Tanuki? */
		if (is_pet(m_ptr) && !is_original_ap(m_ptr))
		{
#ifdef JP
			char *t;
			strcpy(buf, name);
			t = buf;
			while(strncmp(t, "』", 2) && *t) t++;
			if (*t)
			{
				*t = '\0';
				(void)sprintf(desc, "%s？』", buf);
			}
			else
				(void)sprintf(desc, "%s？", name);
#else
			(void)sprintf(desc, "%s?", name);
#endif
		}
		else

		/* It could be a Unique */
#ifdef JP
		if ((r_ptr->flags1 & RF1_UNIQUE) && !(p_ptr->image && !(mode & MD_IGNORE_HALLU)))
#else
        if (((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) && !(p_ptr->image && !(mode & MD_IGNORE_HALLU)))
#endif
		{
			/* Start with the name (thus nominative and objective) */
			if ((m_ptr->mflag2 & MFLAG2_CHAMELEON) && !(mode & MD_TRUE_NAME))
			{
#ifdef JP
				char *t;
				strcpy(buf, name);
				t = buf;
				while (strncmp(t, "』", 2) && *t) t++;
				if (*t)
				{
					*t = '\0';
					(void)sprintf(desc, "%s？』", buf);
				}
				else
					(void)sprintf(desc, "%s？", name);
#else
				(void)sprintf(desc, "%s?", name);
#endif
			}
#if 0 //闘技場でユニークに「もどき」と付けない
			/* Inside monster arena, and it is not your mount */
			else if (p_ptr->inside_battle &&
				 !(p_ptr->riding && (&m_list[p_ptr->riding] == m_ptr)))
			{
				/* It is a fake unique monster */
#ifdef JP
				(void)sprintf(desc, "%sもどき", name);
#else
				(void)sprintf(desc, "fake %s", name);
#endif
			}
#endif
			else
			{
				(void)strcpy(desc, name);
			}
		}

		/* It could be an indefinite monster */
		else if (mode & MD_INDEF_VISIBLE)
		{
			/* XXX Check plurality for "some" */

			/* Indefinite monsters need an indefinite article */
#ifdef JP
			(void)strcpy(desc, "");
#else
			(void)strcpy(desc, is_a_vowel(name[0]) ? "an " : "a ");
#endif

			(void)strcat(desc, name);
		}

		/* It could be a normal, definite, monster */
		else
		{
			/* Definite monsters need a definite article */
			if (is_pet(m_ptr))
#ifdef JP
				(void)strcpy(desc, "あなたの");
#else
				(void)strcpy(desc, "your ");
#endif

			else
#ifdef JP
				(void)strcpy(desc, "");
#else
				(void)strcpy(desc, "the ");
#endif

			(void)strcat(desc, name);
		}

		if (m_ptr->nickname)
		{
#ifdef JP
			sprintf(buf,"「%s」",quark_str(m_ptr->nickname));
#else
			sprintf(buf," called %s",quark_str(m_ptr->nickname));
#endif
			strcat(desc,buf);
		}

		if (p_ptr->riding && (&m_list[p_ptr->riding] == m_ptr))
		{
#ifdef JP
			if (CLASS_RIDING_BACKDANCE && p_ptr->pclass == CLASS_TSUKASA)
				strcat(desc,"(寄生中)");
			else if (CLASS_RIDING_BACKDANCE)
				strcat(desc, "(強化中)");
			else
				strcat(desc, "(乗馬中)");
#else
			if (CLASS_RIDING_BACKDANCE && p_ptr->pclass == CLASS_TSUKASA)
                strcat(desc, "(parasitising)");
			else if (CLASS_RIDING_BACKDANCE)
				strcat(desc, "(empowering)");
			else
				strcat(desc, "(riding)");
#endif
		}

		if ((mode & MD_IGNORE_HALLU) && (m_ptr->mflag2 & MFLAG2_CHAMELEON))
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
			{
#ifdef JP
				strcat(desc,"(カメレオンの王)");
#else
				strcat(desc,"(Chameleon Lord)");
#endif
			}
			else
			{
#ifdef JP
				strcat(desc,"(妖怪狐)");
#else
				strcat(desc,"(Chameleon)");
#endif
			}
		}

		if ((mode & MD_IGNORE_HALLU) && !is_original_ap(m_ptr))
		{
			strcat(desc, format("(%s)", r_name + r_info[m_ptr->r_idx].name));
		}

		/* Handle the Possessive as a special afterthought */
		if (mode & MD_POSSESSIVE)
		{
			/* XXX Check for trailing "s" */

			/* Simply append "apostrophe" and "s" */
#ifdef JP
			(void)strcat(desc, "の");
#else
			(void)strcat(desc, "'s");
#endif
		}
	}
}




/*
 * Learn about a monster (by "probing" it)
 *
 * Return the number of new flags learnt.  -Mogami-
 */
/*:::指定モンスターについての全ての情報を得る　詳細未読*/
int lore_do_probe(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];
	int i, n = 0;
	byte tmp_byte;

	/* Maximal info about awareness */
	if (r_ptr->r_wake != MAX_UCHAR) n++;
	if (r_ptr->r_ignore != MAX_UCHAR) n++;
	r_ptr->r_wake = r_ptr->r_ignore = MAX_UCHAR;

	/* Observe "maximal" attacks */
	for (i = 0; i < 4; i++)
	{
		/* Examine "actual" blows */
		if (r_ptr->blow[i].effect || r_ptr->blow[i].method)
		{
			/* Maximal observations */
			if (r_ptr->r_blows[i] != MAX_UCHAR) n++;
			r_ptr->r_blows[i] = MAX_UCHAR;
		}
	}

	/* Maximal drops */
	tmp_byte =
		(((r_ptr->flags1 & RF1_DROP_4D2) ? 8 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_3D2) ? 6 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_2D2) ? 4 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_1D2) ? 2 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_90)  ? 1 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_60)  ? 1 : 0));

	/* Only "valid" drops */
	if (!(r_ptr->flags1 & RF1_ONLY_GOLD))
	{
		if (r_ptr->r_drop_item != tmp_byte) n++;
		r_ptr->r_drop_item = tmp_byte;
	}
	if (!(r_ptr->flags1 & RF1_ONLY_ITEM))
	{
		if (r_ptr->r_drop_gold != tmp_byte) n++;
		r_ptr->r_drop_gold = tmp_byte;
	}

	/* Observe many spells */
	if (r_ptr->r_cast_spell != MAX_UCHAR) n++;
	r_ptr->r_cast_spell = MAX_UCHAR;

	/* Count unknown flags */
	for (i = 0; i < 32; i++)
	{
		if (!(r_ptr->r_flags1 & (1L << i)) &&
		    (r_ptr->flags1 & (1L << i))) n++;
		if (!(r_ptr->r_flags2 & (1L << i)) &&
		    (r_ptr->flags2 & (1L << i))) n++;
		if (!(r_ptr->r_flags3 & (1L << i)) &&
		    (r_ptr->flags3 & (1L << i))) n++;
		if (!(r_ptr->r_flags4 & (1L << i)) &&
		    (r_ptr->flags4 & (1L << i))) n++;
		if (!(r_ptr->r_flags5 & (1L << i)) &&
		    (r_ptr->flags5 & (1L << i))) n++;
		if (!(r_ptr->r_flags6 & (1L << i)) &&
		    (r_ptr->flags6 & (1L << i))) n++;
		if (!(r_ptr->r_flagsr & (1L << i)) &&
		    (r_ptr->flagsr & (1L << i))) n++;

	///mod140102 モンスター新魔法フラグ追加に伴いflags9追加
		if (!(r_ptr->r_flags9 & (1L << i)) &&
		    (r_ptr->flags9 & (1L << i))) n++;

		/* r_flags7 is actually unused */
#if 0
		if (!(r_ptr->r_flags7 & (1L << i)) &&
		    (r_ptr->flags7 & (1L << i))) n++;
#endif
	}

	/* Know all the flags */
	r_ptr->r_flags1 = r_ptr->flags1;
	r_ptr->r_flags2 = r_ptr->flags2;
	r_ptr->r_flags3 = r_ptr->flags3;
	r_ptr->r_flags4 = r_ptr->flags4;
	r_ptr->r_flags5 = r_ptr->flags5;
	r_ptr->r_flags6 = r_ptr->flags6;
	///mod140102 モンスター新魔法フラグ追加に伴いflags9追加
	r_ptr->r_flags9 = r_ptr->flags9;
	r_ptr->r_flagsr = r_ptr->flagsr;

	/* r_flags7 is actually unused */
	/* r_ptr->r_flags7 = r_ptr->flags7; */

	/* Know about evolution */
	if (!(r_ptr->r_xtra1 & MR1_SINKA)) n++;
	r_ptr->r_xtra1 |= MR1_SINKA;

	/* Update monster recall window */
	if (p_ptr->monster_race_idx == r_idx)
	{
		/* Window stuff */
		p_ptr->window |= (PW_MONSTER);
	}

	/* Return the number of new flags learnt */
	return n;
}


/*
 * Take note that the given monster just dropped some treasure
 *
 * Note that learning the "GOOD"/"GREAT" flags gives information
 * about the treasure (even when the monster is killed for the first
 * time, such as uniques, and the treasure has not been examined yet).
 *
 * This "indirect" method is used to prevent the player from learning
 * exactly how much treasure a monster can drop from observing only
 * a single example of a drop.  This method actually observes how much
 * gold and items are dropped, and remembers that information to be
 * described later by the monster recall code.
 */
/*:::モンスターが落とすアイテムについての思い出更新？未読*/
void lore_treasure(int m_idx, int num_item, int num_gold)
{
	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* If the monster doesn't have original appearance, don't note */
	if (!is_original_ap(m_ptr)) return;

	/* Note the number of things dropped */
	if (num_item > r_ptr->r_drop_item) r_ptr->r_drop_item = num_item;
	if (num_gold > r_ptr->r_drop_gold) r_ptr->r_drop_gold = num_gold;

	/* Hack -- memorize the good/great flags */
	if (r_ptr->flags1 & (RF1_DROP_GOOD)) r_ptr->r_flags1 |= (RF1_DROP_GOOD);
	if (r_ptr->flags1 & (RF1_DROP_GREAT)) r_ptr->r_flags1 |= (RF1_DROP_GREAT);

	/* Update monster recall window */
	if (p_ptr->monster_race_idx == m_ptr->r_idx)
	{
		/* Window stuff */
		p_ptr->window |= (PW_MONSTER);
	}
}


/*:::エルドリッチホラー処理。
 *:::狂気を誘うモンスターを見たときか暗黒魔法の失敗のときここに来る。
 *:::悪夢モードの睡眠は別処理らしい。
 *:::monster_type:モンスターが原因のときにm_ptrが入る　necro:暗黒魔法の失敗のときTRUE */
///mon race res エルドリッチホラー、種族によるエルドリッチホラー耐性、突然変異ペナルティ
void sanity_blast(monster_type *m_ptr, bool necro)
{
	bool happened = FALSE;
	int power = 100;
	int i;

	///mod150918 狂気免疫持ちは完全無効にした
	if(immune_insanity()) return;

	if (p_ptr->inside_battle || !character_dungeon) return;
	//諏訪子冬眠中
	if (p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0]) return;

	/*:::狂気を誘うモンスター*/
	///pendそのうちここ弄ろう。神格レベルのユニークはもっと酷いことしてきて良い
	if (!necro)
	{
		char            m_name[80];
		monster_race    *r_ptr = &r_info[m_ptr->ap_r_idx];

		power = r_ptr->level / 2;

		monster_desc(m_name, m_ptr, 0);

		/*:::友好的な非ユニークはpowerが弱い*/
		if (!(r_ptr->flags1 & RF1_UNIQUE))
		{
			if (r_ptr->flags1 & RF1_FRIENDS)
			power /= 2;
		}
		else power *= 2;

		if (!hack_mind)
			return; /* No effect yet, just loaded... */

		/*:::見えていない敵からは影響を受けない。だがここに来る事ってあるのか？*/
		if (!m_ptr->ml)
			return; /* Cannot see it for some reason */


		/*:::いわゆるparanoia*/
		///mod150124 ニンジャスレイヤーのための例外処理追加
		if (!(r_ptr->flags2 & RF2_ELDRITCH_HORROR) && m_ptr->r_idx != MON_NINJA_SLAYER)
			return; /* oops */
		//NRSは一般人のみにした
		if(m_ptr->r_idx == MON_NINJA_SLAYER && (p_ptr->pclass != CLASS_OUTSIDER && p_ptr->pclass != CLASS_CIVILIAN)) return;


		if (is_pet(m_ptr))
			return; /* Pet eldritch horrors are safe most of the time */

		/*:::敵Levが50以上なら必ずここを通過*/
		if (randint1(100) > power) return;
		/*:::魔法防御で判定　敵レベルが70とかあると多分ほとんど役に立たない*/
		if (saving_throw(p_ptr->skill_sav - power))
		{
			return; /* Save, no adverse effects */
		}
		/*:::幻覚状態のとき変なメッセージが出る。たまにうひょーとか言って幻覚時間が延びる*/
		if (p_ptr->image)
		{
			/* Something silly happens... */
#ifdef JP
			if(m_ptr->r_idx == MON_NINJA_SLAYER)
			{
				msg_format("%sの顔を見てしまった！コワイ！",m_name);
			}
			else if(m_ptr->r_idx == MON_CLOWNPIECE)
			{
				msg_format("「イッツ、ルナティックターイム！」");
			}
			else
			{
				msg_format("%s%sの顔を見てしまった！",
					funny_desc[randint0(MAX_SAN_FUNNY)], m_name);
			}
#else
			if(m_ptr->r_idx == MON_NINJA_SLAYER)
			{
				msg_format("You behold the visage of %s! Scary!",m_name);
			}
			else if(m_ptr->r_idx == MON_CLOWNPIECE)
			{
				msg_format("'It's lunatic time!'");
			}
			else
			{
				msg_format("You behold the %s visage of %s!",
                    funny_desc[randint0(MAX_SAN_FUNNY)], m_name);
			}
#endif


			if (one_in_(3))
			{
				msg_print(funny_comments[randint0(MAX_SAN_COMMENT)]);
				p_ptr->image = p_ptr->image + randint1(r_ptr->level);
			}

			return; /* Never mind; we can't see it clearly enough */
		}

		/* Something frightening happens... */
#ifdef JP
		if(m_ptr->r_idx == MON_NINJA_SLAYER)
		{
			msg_print("アイエエエ！ニンジャ！？ニンジャナンデ！？");
		}
		else if(m_ptr->r_idx == MON_CLOWNPIECE)
		{
			msg_format("%sの松明の光が目に焼き付いた・・",m_name);
		}
		else
		{
			msg_format("%s%sの顔を見てしまった！",
				horror_desc[randint0(MAX_SAN_HORROR)], m_name);
		}
#else
		if(m_ptr->r_idx == MON_NINJA_SLAYER)
		{
			msg_print("Aiieeee! Ninja!? Ninjaa!?");
		}
		else if(m_ptr->r_idx == MON_CLOWNPIECE)
		{
			msg_format("The light from the torch of %s burns into your eyes...",m_name);
		}
		else
		{
			msg_format("You behold the %s visage of %s!",
                horror_desc[randint0(MAX_SAN_HORROR)], m_name);
		}
#endif
		/*:::モンスターの思い出に「狂気を誘う」フラグを追加*/
		if(m_ptr->r_idx != MON_NINJA_SLAYER)
			r_ptr->r_flags2 |= RF2_ELDRITCH_HORROR;

		/* Demon characters are unaffected */
		/*:::ここはきちんと書き換えよう*/
		//if (prace_is_(RACE_IMP)  || (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_DEMON)) return;
		if (p_ptr->wizard) return;

		/* Undead characters are 50% likely to be unaffected */
		/*
		if (prace_is_(RACE_SKELETON) || prace_is_(RACE_ZOMBIE)
			|| prace_is_(RACE_VAMPIRE) || prace_is_(RACE_SPECTRE) ||
		    (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_UNDEAD))
		{
			if (saving_throw(25 + p_ptr->lev)) return;
		}
		*/
		///mod131228 狂気耐性でsanityblastに抵抗
		///pend sanityblast抵抗　敵が高レベルだともっと回避しづらく凶悪にしてもいい
		if(p_ptr->resist_insanity && saving_throw(25 + p_ptr->lev))
		{
			msg_print(_("しかしあなたの精神は影響を受けなかった。",
                        "But you maintain your sanity."));
			return;
		}
	}
	/*:::ネクロノミコン　ここは種族処理とかないらしい*/
	///mod131231 ネクロノミコン処理だったが狂気打撃用に流用しよう
	else
	{
#ifdef JP
msg_print("精神が蝕まれる！");

		//msg_print("ネクロノミコンを読んで正気を失った！");
#else
		msg_print("Your mind is being eroded!");
#endif

	}
	/*:::ここから実被害処理*/
	if (!saving_throw(p_ptr->skill_sav - power)) /* Mind blast */
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

	if (!saving_throw(p_ptr->skill_sav - power)) /* Lose int & wis */
	{
		do_dec_stat(A_INT);
		do_dec_stat(A_WIS);
		return;
	}

	if (!saving_throw(p_ptr->skill_sav - power)) /* Brain smash */
	{
		if (!p_ptr->resist_conf)
		{
			(void)set_confused(p_ptr->confused + randint0(4) + 4);
		}
		if (!p_ptr->free_act)
		{
			(void)set_paralyzed(p_ptr->paralyzed + randint0(4) + 4);
		}
		while (randint0(100) > p_ptr->skill_sav)
			(void)do_dec_stat(A_INT);
		while (randint0(100) > p_ptr->skill_sav)
			(void)do_dec_stat(A_WIS);
		if (!p_ptr->resist_chaos)
		{
			(void)set_image(p_ptr->image + randint0(250) + 150);
		}
		return;
	}

	if (!saving_throw(p_ptr->skill_sav - power)) /* Amnesia */
	{

		if (lose_all_info())
#ifdef JP
msg_print("あまりの恐怖に全てのことを忘れてしまった！");
#else
			msg_print("You forget everything in your utmost terror!");
#endif

		return;
	}

	if (saving_throw(p_ptr->skill_sav - power))
	{
		return;
	}

	//蓬莱人は変異無効
	if(p_ptr->prace == RACE_HOURAI) return;


	///mod160225 変異付与ルーチンを見直した
	get_bad_mental_mutation();

#if 0
	/* Else gain permanent insanity */
	if ((p_ptr->muta3 & MUT3_MORONIC) && /*(p_ptr->muta2 & MUT2_BERS_RAGE) &&*/
		((p_ptr->muta2 & MUT2_COWARDICE) || (p_ptr->resist_fear)) &&
		((p_ptr->muta2 & MUT2_HALLU) || (p_ptr->resist_chaos)))
	{
		/* The poor bastard already has all possible insanities! */
		return;
	}


	while (!happened)
	{
		switch (randint1(21))
		{
			case 1:
				if (!(p_ptr->muta3 & MUT3_MORONIC) && one_in_(5))
				{
					if ((p_ptr->stat_use[A_INT] < 4) && (p_ptr->stat_use[A_WIS] < 4))
					{
#ifdef JP
msg_print("あなたは完璧な馬鹿になったような気がした。しかしそれは元々だった。");
#else
						msg_print("You turn into an utter moron!");
#endif
					}
					else
					{
#ifdef JP
msg_print("あなたは完璧な馬鹿になった！");
#else
						msg_print("You turn into an utter moron!");
#endif
					}

					if (p_ptr->muta3 & MUT3_HYPER_INT)
					{
#ifdef JP
msg_print("あなたの脳は生体コンピュータではなくなった。");
#else
						msg_print("Your brain is no longer a living computer.");
#endif

						p_ptr->muta3 &= ~(MUT3_HYPER_INT);
					}
					p_ptr->muta3 |= MUT3_MORONIC;
					happened = TRUE;
				}
				break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
				if (!(p_ptr->muta2 & MUT2_COWARDICE) && !p_ptr->resist_fear)
				{
#ifdef JP
msg_print("あなたは閉所恐怖症になった！");
#else
					msg_print("You become paranoid!");
#endif


					/* Duh, the following should never happen, but anyway... */
					if (p_ptr->muta3 & MUT3_FEARLESS)
					{
#ifdef JP
msg_print("あなたはもう恐れ知らずではなくなった。");
#else
						msg_print("You are no longer fearless.");
#endif

						p_ptr->muta3 &= ~(MUT3_FEARLESS);
					}

					p_ptr->muta2 |= MUT2_COWARDICE;
					happened = TRUE;
				}
				break;
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
				if (!(p_ptr->muta2 & MUT2_HALLU) && !p_ptr->resist_chaos)
				{
#ifdef JP
msg_print("幻覚をひき起こす精神錯乱に陥った！");
#else
					msg_print("You are afflicted by a hallucinatory insanity!");
#endif

					p_ptr->muta2 |= MUT2_HALLU;
					happened = TRUE;
				}
				break;
			default:
				if (!(p_ptr->muta2 & MUT2_BERS_RAGE))
				{
#ifdef JP
msg_print("制御不能な激情を感じる！");
#else
					msg_print("You become subject to fits of berserk rage!");
#endif

					p_ptr->muta2 |= MUT2_BERS_RAGE;
					happened = TRUE;
				}
				break;
		}
	}
#endif
	/*:::変異などを食らったときのためのパラメータ関係再計算？*/
	p_ptr->update |= PU_BONUS;
	handle_stuff();
}

//精神的な悪性突然変異を受ける。sanityblastとhave_nightmareで重複してるのがなんか嫌で分離共有
void get_bad_mental_mutation(void)
{
	int i;
	for(i=0;i<3;i++)
	{
		int roll = randint0(100);

		if(roll < 20)
		{
			break;
		}
		else if(roll < 35)
		{
			if(!p_ptr->resist_chaos && (p_ptr->muta2 & MUT2_HALLU)) gain_random_mutation(79);//MUT2_HALLU
		}
		else if(roll < 50)
		{
			if(!p_ptr->resist_fear && (p_ptr->muta2 & MUT2_COWARDICE)) gain_random_mutation(76);//MUT2_COWARD
		}
		else if(roll < 65)
		{
			 if(!p_ptr->resist_conf && p_ptr->muta2 & MUT2_BERS_RAGE) gain_random_mutation(75);//MUT2_BERS_RAGE
		}
		else if(roll < 75)
		{
			if(p_ptr->muta2 & MUT2_SPEED_FLUX) gain_random_mutation(90);//MUT2_SPEED_FLUX
		}
		else if(roll < 85)
		{
			if(p_ptr->muta2 & MUT2_WARNING) gain_random_mutation(113);//MUT2_WARNING
		}
		else if(roll < 90)
		{
			if(p_ptr->muta2 & MUT2_WALK_SHAD) gain_random_mutation(112);//MUT2_WALK_SHAD
		}
		else if(roll < 95)
		{
			if(p_ptr->muta3 & MUT3_MORONIC) gain_random_mutation(129);//MUT3_MORONIC
		}
		else
		{
			if(p_ptr->muta2 & MUT2_ATT_DEMON) gain_random_mutation(87);//MUT2_ATT_DEMON
		}
	}


}


/*
 * This function updates the monster record of the given monster
 *
 * This involves extracting the distance to the player (if requested),
 * and then checking for visibility (natural, infravision, see-invis,
 * telepathy), updating the monster visibility flag, redrawing (or
 * erasing) the monster when its visibility changes, and taking note
 * of any interesting monster flags (cold-blooded, invisible, etc).
 *:::可視範囲、感知手段などの変更に応じて「このモンスターは感知されている」フラグを操作する
 *
 * Note the new "mflag" field which encodes several monster state flags,
 * including "view" for when the monster is currently in line of sight,
 * and "mark" for when the monster is currently visible via detection.
 *
 * The only monster fields that are changed here are "cdis" (the
 * distance from the player), "ml" (visible to the player), and
 * "mflag" (to maintain the "MFLAG_VIEW" flag).
 *
 * Note the special "update_monsters()" function which can be used to
 * call this function once for every monster.
 *
 * Note the "full" flag which requests that the "cdis" field be updated,
 * this is only needed when the monster (or the player) has moved.
 *
 * Every time a monster moves, we must call this function for that
 * monster, and update the distance, and the visibility.  Every time
 * the player moves, we must call this function for every monster, and
 * update the distance, and the visibility.  Whenever the player "state"
 * changes in certain ways ("blindness", "infravision", "telepathy",
 * and "see invisible"), we must call this function for every monster,
 * and update the visibility.
 *:::モンスターが動くたびにこの関数が呼ばれ、距離と可視不可視の情報を更新する必要がある
 *:::＠が動いたり光源や感知関係のステータスが変わるたびに全てのモンスターに関してこの関数で処理する必要がある
 *
 * Routines that change the "illumination" of a grid must also call this
 * function for any monster in that grid, since the "visibility" of some
 * monsters may be based on the illumination of their grid.
 *
 * Note that this function is called once per monster every time the
 * player moves.  When the player is running, this function is one
 * of the primary bottlenecks, along with "update_view()" and the
 * "process_monsters()" code, so efficiency is important.
 *
 * Note the optimized "inline" version of the "distance()" function.
 *
 * A monster is "visible" to the player if (1) it has been detected
 * by the player, (2) it is close to the player and the player has
 * telepathy, or (3) it is close to the player, and in line of sight
 * of the player, and it is "illuminated" by some combination of
 * infravision, torch light, or permanent light (invisible monsters
 * are only affected by "light" if the player can see invisible).
 *:::モンスターがvisibleになるのは、＠が感知したとき、ESPを持ってるとき、視界に入ったとき
 *
 *
 * Monsters which are not on the current panel may be "visible" to
 * the player, and their descriptions will include an "offscreen"
 * reference.  Currently, offscreen monsters cannot be targetted
 * or viewed directly, but old targets will remain set.  XXX XXX
 *
 * The player can choose to be disturbed by several things, including
 * "disturb_move" (monster which is viewable moves in some way), and
 * "disturb_near" (monster which is "easily" viewable moves in some
 * way).  Note that "moves" includes "appears" and "disappears".
 */

/*:::full:これがtrueのとき＠とこのモンスターとの距離(m_ptr->cdis)を計算して更新する　移動を伴わない呼び出しのときはfalseでいい？*/
/*:::入身など移動系特技のときも呼ばれてるはず？実装時注意*/
///mon item モンスター感知関係　ESPやエルドリッチホラー
void update_mon(int m_idx, bool full)
{
	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	bool do_disturb = disturb_move;

	int d;

	/* Current location */
	int fy = m_ptr->fy;
	int fx = m_ptr->fx;

	/* Seen at all */
	bool flag = FALSE;

	/* Seen by vision */
	bool easy = FALSE;

	/* Non-Ninja player in the darkness */
	/*:::暗闇の洞窟？*/
	bool in_darkness = (d_info[dungeon_type].flags1 & DF1_DARKNESS) && !p_ptr->see_nocto;

	/* Do disturb? */
	/*:::外見が高レベルのモンスター、かつ倒したことがある（＠がレベルを知っている）場合、＠の行動を中断させる*/
	if (disturb_high)
	{
		monster_race *ap_r_ptr = &r_info[m_ptr->ap_r_idx];

		if (!ap_r_ptr->r_tkills) do_disturb = TRUE;

		if (ap_r_ptr->r_tkills && ap_r_ptr->level >= p_ptr->lev)
			do_disturb = TRUE;
	}

	/* Compute distance */
	/*:::monと＠との距離を更新*/
	if (full)
	{
		/* Distance components */
		int dy = (py > fy) ? (py - fy) : (fy - py);
		int dx = (px > fx) ? (px - fx) : (fx - px);

		/* Approximate distance */
		/*:::距離の近似値：△x,△yの小さいほうを半分にして足している*/
		d = (dy > dx) ? (dy + (dx>>1)) : (dx + (dy>>1));

		/* Restrict distance */
		if (d > 255) d = 255;

		if (!d) d = 1;

		/* Save the distance */
		m_ptr->cdis = d;
	}
	/* Extract distance */
	/*:::fullがfalseのとき過去の値を使う　移動を伴わない呼び出しならこちらでいい？*/
	else
	{
		/* Extract the distance */
		d = m_ptr->cdis;
	}


	/* Detected */
	/*:::モンスター感知などで感知されたモンスター？*/
	if (m_ptr->mflag2 & (MFLAG2_MARK)) flag = TRUE;

	//v1.1.35 ネムノの縄張りに入った敵は感知される
	if(cave[m_ptr->fy][m_ptr->fx].cave_xtra_flag & CAVE_XTRA_FLAG_NEMUNO)
		flag = TRUE;

	/* Nearby */
	/*:::視界内（になりうる距離）にいるとき*/
	if (d <= (in_darkness ? MAX_SIGHT / 2 : MAX_SIGHT))
	{
		/*:::暗闇の洞窟では5マスしかテレパスが効かない*/
		if (!in_darkness || (d <= MAX_SIGHT / 4))
		{
			/*:::夢想の型のときはテレパス不可の近くのモンスターが壁越しに見える*/
			if (p_ptr->special_defense & KATA_MUSOU)
			{
				/* Detectable */
				flag = TRUE;

				/*:::夢想の型中でタヌキでも幻覚でもない場合？モンスターの精神フラグ処理らしいが　　的確に魔法を使う、などの部分がわかるのか？*/
				if (is_original_ap(m_ptr) && !p_ptr->image)
				{
					/* Hack -- Memorize mental flags */
					if (r_ptr->flags2 & (RF2_SMART)) r_ptr->r_flags2 |= (RF2_SMART);
					if (r_ptr->flags2 & (RF2_STUPID)) r_ptr->r_flags2 |= (RF2_STUPID);
				}
			}

			/* Basic telepathy */
			/* Snipers get telepathy when they concentrate deeper */
			else if (p_ptr->telepathy)
			{
				/* Empty mind, no telepathy */
				if (r_ptr->flags2 & (RF2_EMPTY_MIND))
				{
					/* Memorize flags */
					/*:::EMPTY_MINDフラグの敵をテレパシーで感知すればテレパシーで感知できないことがわかる？*/
					if (is_original_ap(m_ptr) && !p_ptr->image) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
				}

				/* Weird mind, occasional telepathy */
				else if (r_ptr->flags2 & (RF2_WEIRD_MIND))
				{
					/* One in ten individuals are detectable */
					/*:::One_in_()などの判定ではターン毎に感知できたりできなかったりするので不適なのだと思われる*/
					//if ((m_idx % 10) == 5)
					//v1.1.81 WEIRD_MINDの感知ルーチン変更
					if(m_ptr->mon_random_number % 10 == turn % 10)
					{
						/* Detectable */
						flag = TRUE;

						if (is_original_ap(m_ptr) && !p_ptr->image)
						{
							/* Memorize flags */
							r_ptr->r_flags2 |= (RF2_WEIRD_MIND);

							/* Hack -- Memorize mental flags */
							/*:::WEIRDかつSMARTなんて敵も居るのか？*/
							if (r_ptr->flags2 & (RF2_SMART)) r_ptr->r_flags2 |= (RF2_SMART);
							if (r_ptr->flags2 & (RF2_STUPID)) r_ptr->r_flags2 |= (RF2_STUPID);
						}
					}
				}

				/* Normal mind, allow telepathy */
				else
				{
					/* Detectable */
					flag = TRUE;

					if (is_original_ap(m_ptr) && !p_ptr->image)
					{
						/* Hack -- Memorize mental flags */
						if (r_ptr->flags2 & (RF2_SMART)) r_ptr->r_flags2 |= (RF2_SMART);
						if (r_ptr->flags2 & (RF2_STUPID)) r_ptr->r_flags2 |= (RF2_STUPID);
					}
				}
			}
			/*:::種族ESPで感知したらモンスターの種族分類を記憶する*/
			/* Magical sensing */
			///mon ESPによる感知処理
			if ((p_ptr->esp_animal) && (r_ptr->flags3 & (RF3_ANIMAL)))
			{
				flag = TRUE;
				if (is_original_ap(m_ptr) && !p_ptr->image) r_ptr->r_flags3 |= (RF3_ANIMAL);
			}

			/* Magical sensing */
			if ((p_ptr->esp_undead) && (r_ptr->flags3 & (RF3_UNDEAD)))
			{
				flag = TRUE;
				if (is_original_ap(m_ptr) && !p_ptr->image) r_ptr->r_flags3 |= (RF3_UNDEAD);
			}

			/* Magical sensing */
			if ((p_ptr->esp_demon) && (r_ptr->flags3 & (RF3_DEMON)))
			{
				flag = TRUE;
				if (is_original_ap(m_ptr) && !p_ptr->image) r_ptr->r_flags3 |= (RF3_DEMON);
			}
			///mod131231 モンスターフラグ変更
			/* Magical sensing */

			if ((p_ptr->esp_kwai) && (r_ptr->flags3 & (RF3_KWAI)))
			{
				flag = TRUE;
				if (is_original_ap(m_ptr) && !p_ptr->image) r_ptr->r_flags3 |= (RF3_KWAI);
			}

			/* Magical sensing */

			if ((p_ptr->esp_deity) && (r_ptr->flags3 & (RF3_DEITY)))
			{
				flag = TRUE;
				if (is_original_ap(m_ptr) && !p_ptr->image) r_ptr->r_flags3 |= (RF3_DEITY);
			}

			/* Magical sensing */
			//DEMIHUMANは人ESPで7割感知できる
			//v1.1.81 感知ルーチン変更
			//if ((p_ptr->esp_human) && (r_ptr->flags3 & (RF3_DEMIHUMAN)) && () < 7)
			if ((p_ptr->esp_human) && (r_ptr->flags3 & (RF3_DEMIHUMAN)) && (m_ptr->mon_random_number % 7 == turn % 7))
			{
				flag = TRUE;
				if (is_original_ap(m_ptr) && !p_ptr->image) r_ptr->r_flags3 |= (RF3_DEMIHUMAN);
			}

			/* Magical sensing */
			if ((p_ptr->esp_dragon) && (r_ptr->flags3 & (RF3_DRAGON)))
			{
				flag = TRUE;
				if (is_original_ap(m_ptr) && !p_ptr->image) r_ptr->r_flags3 |= (RF3_DRAGON);
			}

			/* Magical sensing */
			///mod131231 モンスターフラグ変更
			if ((p_ptr->esp_human) && (r_ptr->flags3 & (RF3_HUMAN)))
			//if ((p_ptr->esp_human) && (r_ptr->flags2 & (RF2_HUMAN)))
			{
				flag = TRUE;
				//if (is_original_ap(m_ptr) && !p_ptr->image) r_ptr->r_flags2 |= (RF2_HUMAN);
				if (is_original_ap(m_ptr) && !p_ptr->image) r_ptr->r_flags3 |= (RF3_HUMAN);
			}

			/* Magical sensing */
			if ((p_ptr->esp_evil) && (r_ptr->flags3 & (RF3_ANG_CHAOS)))
			{
				flag = TRUE;
				if (is_original_ap(m_ptr) && !p_ptr->image) r_ptr->r_flags3 |= (RF3_ANG_CHAOS);
			}

			/* Magical sensing */
			if ((p_ptr->esp_good) && (r_ptr->flags3 & (RF3_ANG_COSMOS)))
			{
				flag = TRUE;
				if (is_original_ap(m_ptr) && !p_ptr->image) r_ptr->r_flags3 |= (RF3_ANG_COSMOS);
			}

			/* Magical sensing */
			/*:::無生物感知　ここにDEMONやUNDEADのフラグを持ってくる意味は何だろう？昔のバージョンでは感知できたが消されたのか？*/
			if ((p_ptr->esp_nonliving) &&
			    ((r_ptr->flags3 & (RF3_DEMON | RF3_UNDEAD | RF3_NONLIVING)) == RF3_NONLIVING))
			{
				flag = TRUE;
				if (is_original_ap(m_ptr) && !p_ptr->image) r_ptr->r_flags3 |= (RF3_NONLIVING);
			}

			/* Magical sensing */
			if ((p_ptr->esp_unique) && ((r_ptr->flags1 & (RF1_UNIQUE)) ||(r_ptr->flags7 & (RF7_UNIQUE2))) )
			{
				flag = TRUE;
				if (is_original_ap(m_ptr) && !p_ptr->image) r_ptr->r_flags1 |= (RF1_UNIQUE);
			}

			//v1.1.66 磨弓は埴輪を感知する
			if (p_ptr->pclass == CLASS_MAYUMI && (r_ptr->flags3 & (RF3_HANIWA))) flag = TRUE;
			if (p_ptr->pclass == CLASS_KEIKI && (r_ptr->flags3 & (RF3_HANIWA))) flag = TRUE;


			if(CHECK_FAIRY_TYPE == 21 && (r_ptr->flags3 & (RF3_FAIRY))) flag = TRUE;

			//v1.1.77 お燐(専用性格)の特技「追跡」の対象のモンスター

			if (is_special_seikaku(SEIKAKU_SPECIAL_ORIN) && m_idx == p_ptr->magic_num1[0]) flag = TRUE;

			//椛Lv35以降の完全感知　全てを感知するがモンスターの属性について何も学べない
			//レーダーセンスも同じ処理にする
			if(p_ptr->pclass == CLASS_MOMIZI && p_ptr->lev > 34
			|| p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->lev > 35 && p_ptr->realm1 == TV_BOOK_FORESEE
			||(p_ptr->pclass == CLASS_REIMU && p_ptr->lev > 44 && osaisen_rank() > 8)
			||(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 74)
				|| p_ptr->radar_sense)
			{
				flag = TRUE;
			}
			if(p_ptr->mimic_form == MIMIC_DRAGON && p_ptr->mimic_dragon_rank > 169)
			{
				flag = TRUE;
			}
			//スターサファイアのモンスター感知
			//v1.1.53 酔っているとき探知の精度が鈍ることにした
			if((p_ptr->pclass == CLASS_STAR || p_ptr->pclass == CLASS_3_FAIRIES))
			{
				int range = p_ptr->lev / 5 + 10;
				if (p_ptr->alcohol > DRANK_2) range /= 2;
				if (p_ptr->alcohol > DRANK_3) range /= 2;

				if (d <= range)
				{
					if (p_ptr->lev > 29) flag = TRUE;
					else if (monster_living(r_ptr)) flag = TRUE;

				}

			}


			///mod150116 村紗は水の近くにいるモンスターが見えるようにしてみる
			if(p_ptr->pclass == CLASS_MURASA && p_ptr->lev > 19)
			{
				int i,xx,yy;
				for(i=0;i<8;i++)
				{
						yy = m_ptr->fy + ddy_cdd[i];
						xx = m_ptr->fx + ddx_cdd[i];
						if(!in_bounds(yy,xx)) continue;
						if(cave_have_flag_bold((yy), (xx), FF_WATER)) flag = TRUE;
				}

			}
			//高レベルのヤマメは巣に引っかかった敵を感知する
			if(p_ptr->pclass == CLASS_YAMAME && p_ptr->lev > 29 && cave_have_flag_bold(fy,fx,FF_SPIDER_NEST_1)) flag = TRUE;




		}
		/*:::テレパス処理終了　視界内処理へ*/

		/* Normal line of sight, and not blind */
		/*:::Qここよく解らない。視線が通ってるかチェックするにしては短すぎるように思うが*/
		/*:::ひょっとして、＠が動くたびに周り全てのcave[][].infoのCAVE_VIEWが立ったり落ちたりするのか？*/
		if (player_has_los_bold(fy, fx) && !p_ptr->blind)
		{
			bool do_invisible = FALSE;
			bool do_cold_blood = FALSE;

			/* Snipers can see targets in darkness when they concentrate deeper */
			///class スナイパー特殊処理
			if ((p_ptr->pclass == CLASS_SNIPER) && p_ptr->concent >= CONCENT_RADAR_THRESHOLD)
			{
				/* Easy to see */
				easy = flag = TRUE;
			}

			/* Use "infravision" */
			if (d <= p_ptr->see_infra)
			{
				/* Handle "cold blooded" monsters */
				/*:::QここのAURA_FIREに意味はあるのか？昔は透明で冷血動物で火炎オーラ付きのモンスターが居てそれを赤外線視力で感知できたが今は消されてる・・？*/
				if ((r_ptr->flags2 & (RF2_COLD_BLOOD | RF2_AURA_FIRE)) == RF2_COLD_BLOOD)
				{
					/* Take note */
					do_cold_blood = TRUE;
				}

				/* Handle "warm blooded" monsters */
				else
				{
					/* Easy to see */
					easy = flag = TRUE;
				}
			}

			/* Use "illumination" */
			/*:::光源による可視範囲処理*/
			if (player_can_see_bold(fy, fx))
			{
				/* Handle "invisible" monsters */
				if (r_ptr->flags2 & (RF2_INVISIBLE))
				{
					/* Take note */
					do_invisible = TRUE;

					/* See invisible */
					if (p_ptr->see_inv)
					{
						/* Easy to see */
						easy = flag = TRUE;
					}
				}

				/* Handle "normal" monsters */
				else
				{
					/* Easy to see */
					easy = flag = TRUE;
				}
			}

			/* Visible */
			if (flag)
			{
				if (is_original_ap(m_ptr) && !p_ptr->image)
				{
					/* Memorize flags */
					if (do_invisible) r_ptr->r_flags2 |= (RF2_INVISIBLE);
					if (do_cold_blood) r_ptr->r_flags2 |= (RF2_COLD_BLOOD);
				}
			}
		}
	}


	/* The monster is now visible */
	if (flag)
	{
		/* It was previously unseen */
		if (!m_ptr->ml)
		{
			/* Mark as visible */
			/*:::モンスターの見えてるフラグON*/
			m_ptr->ml = TRUE;

			/* Draw the monster */
			/*:::特定マスを再描画する*/
			lite_spot(fy, fx);

			/* Update health bar as needed */
			/*:::ターゲットや乗馬中ペットがいる場合再描画対象に加える*/
			if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
			if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

			/* Hack -- Count "fresh" sightings */
			/*:::この種族のmonを見た回数をカウントしている.*/
				if (!p_ptr->image)
			{
				if ((m_ptr->ap_r_idx == MON_KAGE) && (r_info[MON_KAGE].r_sights < MAX_SHORT))
					r_info[MON_KAGE].r_sights++;
				else if (is_original_ap(m_ptr) && (r_ptr->r_sights < MAX_SHORT))
					r_ptr->r_sights++;
			}

			/* Eldritch Horror */
			if (r_info[m_ptr->ap_r_idx].flags2 & RF2_ELDRITCH_HORROR)
			{
				sanity_blast(m_ptr, FALSE);
			}
			///mod150124 ニンジャスレイヤーのNRS 人間の一般人、外来人のみ対象
			if(m_ptr->r_idx == MON_NINJA_SLAYER && p_ptr->prace == RACE_HUMAN && (p_ptr->pclass == CLASS_OUTSIDER || p_ptr->pclass == CLASS_CIVILIAN))
			{
				sanity_blast(m_ptr, FALSE);
			}
			//v1.1.17 純狐が月面勢力を見ると狂戦士化
			if(p_ptr->pclass == CLASS_JUNKO && (r_info[m_ptr->ap_r_idx].flags3 & RF3_GEN_MOON)
				&& !is_pet(m_ptr) && hack_mind && !p_ptr->inside_battle)
			{
					char            m_name[80];
					monster_desc(m_name, m_ptr, 0x00);

					msg_format(_("憎き%sの顔を見てしまった！", "You behold the hateful visage of %s!"),m_name);
					set_shero(10+randint1(10),FALSE);

			}

			/* Disturb on appearance */
			/*:::「視界内のモンスターが動いたら行動を中止する」「視界内のペットが動いたら行動を中止する」オプションの処理*/
			if (disturb_near && (projectable(m_ptr->fy, m_ptr->fx, py, px) && projectable(py, px, m_ptr->fy, m_ptr->fx)))
			{
				if (disturb_pets || is_hostile(m_ptr))
					disturb(1, 1);
			}
		}
	}

	/* The monster is not visible */
	else
	{
		/* It was previously seen */
		/*:::モンスター感知した次のターン、もしくはESPや光源範囲から外れたときここに来るはず*/
		if (m_ptr->ml)
		{
			/* Mark as not visible */
			m_ptr->ml = FALSE;

			/* Erase the monster */
			lite_spot(fy, fx);

			/* Update health bar as needed */
			if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
			if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

			/* Disturb on disappearance */
			if (do_disturb)
			{
				if (disturb_pets || is_hostile(m_ptr))
					disturb(1, 1);
			}
		}
	}


	/* The monster is now easily visible */
	/*:::感知やESPでなく直接見えている場合の処理*/
	if (easy)
	{
		/* Change */
		if (!(m_ptr->mflag & (MFLAG_VIEW)))
		{
			/* Mark as easily visible */
			m_ptr->mflag |= (MFLAG_VIEW);

			/* Disturb on appearance */
			if (do_disturb)
			{
				if (disturb_pets || is_hostile(m_ptr))
					disturb(1, 1);
			}
		}
	}

	/* The monster is not easily visible */
	else
	{
		/* Change */
		if (m_ptr->mflag & (MFLAG_VIEW))
		{
			/* Mark as not easily visible */
			m_ptr->mflag &= ~(MFLAG_VIEW);

			/* Disturb on disappearance */
			if (do_disturb)
			{
				if (disturb_pets || is_hostile(m_ptr))
					disturb(1, 1);
			}
		}
	}
}


/*
 * This function simply updates all the (non-dead) monsters (see above).
 */
void update_monsters(bool full)
{
	int i;

	/* Update each (live) monster */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Update the monster */
		update_mon(i, full);
	}
}


/*
 * Hack -- the index of the summoning monster
 */
static bool monster_hook_chameleon_lord(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];
	monster_type *m_ptr = &m_list[chameleon_change_m_idx];
	monster_race *old_r_ptr = &r_info[m_ptr->r_idx];

	if (!(r_ptr->flags1 & (RF1_UNIQUE))) return FALSE;
	if (r_ptr->flags7 & (RF7_FRIENDLY | RF7_CHAMELEON)) return FALSE;
	///pend カメレオン特殊処理　とりあえず消す
	//if (ABS(r_ptr->level - r_info[MON_CHAMELEON_K].level) > 5) return FALSE;

	if ((r_ptr->blow[0].method == RBM_EXPLODE) || (r_ptr->blow[1].method == RBM_EXPLODE) || (r_ptr->blow[2].method == RBM_EXPLODE) || (r_ptr->blow[3].method == RBM_EXPLODE))
		return FALSE;

	if (!monster_can_cross_terrain(cave[m_ptr->fy][m_ptr->fx].feat, r_ptr, 0)) return FALSE;

	/* Not born */
	if (!(old_r_ptr->flags7 & RF7_CHAMELEON))
	{
		if (monster_has_hostile_align(m_ptr, 0, 0, r_ptr)) return FALSE;
	}

	/* Born now */
	else if (summon_specific_who > 0)
	{
		if (monster_has_hostile_align(&m_list[summon_specific_who], 0, 0, r_ptr)) return FALSE;
	}

	return TRUE;
}

///mod140629 カメレオン変身先選定 狸と可変パラメータモンスターには変身しないようにしておく
static bool monster_hook_chameleon(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];
	monster_type *m_ptr = &m_list[chameleon_change_m_idx];
	monster_race *old_r_ptr = &r_info[m_ptr->r_idx];

	if (r_ptr->flags1 & (RF1_UNIQUE)) return FALSE;
	if (r_ptr->flags2 & RF2_MULTIPLY) return FALSE;
	if (r_ptr->flags7 & (RF7_FRIENDLY | RF7_CHAMELEON)) return FALSE;
	if (r_ptr->flags7 & (RF7_VARIABLE | RF7_TANUKI)) return FALSE;

	//v1.1.91 召喚を持つモンスターに変身しないようにする。狐狸戦争クエストのバランス調整用
	if (r_ptr->flags4 & RF4_SUMMON_MASK) return FALSE;
	if (r_ptr->flags5 & RF5_SUMMON_MASK) return FALSE;
	if (r_ptr->flags6 & RF6_SUMMON_MASK) return FALSE;
	if (r_ptr->flags9 & RF9_SUMMON_MASK) return FALSE;


	///mod160316 EASYなどのとき発生抑止モンスターに変身しないようにする
	if(!check_rest_f50(r_idx) ) return FALSE;

	//v1.1.91 フロアレベル/2以下にはならない
	if (r_ptr->level < MIN(dun_level / 2, 40)) return FALSE;

	if ((r_ptr->blow[0].method == RBM_EXPLODE) || (r_ptr->blow[1].method == RBM_EXPLODE) || (r_ptr->blow[2].method == RBM_EXPLODE) || (r_ptr->blow[3].method == RBM_EXPLODE))
		return FALSE;

	if (!monster_can_cross_terrain(cave[m_ptr->fy][m_ptr->fx].feat, r_ptr, 0)) return FALSE;

	/* Not born */
	if (!(old_r_ptr->flags7 & RF7_CHAMELEON))
	{
		if ((old_r_ptr->flags3 & RF3_ANG_COSMOS) && !(r_ptr->flags3 & RF3_ANG_COSMOS)) return FALSE;
		if ((old_r_ptr->flags3 & RF3_ANG_CHAOS) && !(r_ptr->flags3 & RF3_ANG_CHAOS)) return FALSE;
		if (!(old_r_ptr->flags3 & (RF3_ANG_COSMOS | RF3_ANG_CHAOS)) && (r_ptr->flags3 & (RF3_ANG_COSMOS | RF3_ANG_CHAOS))) return FALSE;
	}

	/* Born now */
	else if (summon_specific_who > 0)
	{
		if (monster_has_hostile_align(&m_list[summon_specific_who], 0, 0, r_ptr)) return FALSE;
	}

	return (*(get_monster_hook()))(r_idx);
}


/*:::カメレオン・カメレオンの王の変身処理*/
void choose_new_monster(int m_idx, bool born, int r_idx)
{
	int oldmaxhp;
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr;
	char old_m_name[80];
	bool old_unique = FALSE;
	int old_r_idx = m_ptr->r_idx;

	if (r_info[m_ptr->r_idx].flags1 & RF1_UNIQUE)
		old_unique = TRUE;
	///pend カメレオンの王特殊処理　とりあえず無効化
	//if (old_unique && (r_idx == MON_CHAMELEON)) r_idx = MON_CHAMELEON_K;
	r_ptr = &r_info[r_idx];

	monster_desc(old_m_name, m_ptr, 0);

	if (!r_idx)
	{
		int level;
		/*:::変身先のモンスターを選ぶための初期化処理*/
		chameleon_change_m_idx = m_idx;
		if (old_unique)
			get_mon_num_prep(monster_hook_chameleon_lord, NULL);
		else
			get_mon_num_prep(monster_hook_chameleon, NULL);

		if (old_unique)
			///pend カメレオンの王特殊処理　とりあえず無効化
			//level = r_info[MON_CHAMELEON_K].level;
			level = 50;
		else if (!dun_level)
			level = wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].level;
		else
			level = dun_level;

		if (d_info[dungeon_type].flags1 & DF1_CHAMELEON) level+= 2+randint1(3);

		r_idx = get_mon_num(level);
		r_ptr = &r_info[r_idx];

		chameleon_change_m_idx = 0;

		/* Paranoia */
		if (!r_idx) return;
	}

	if (is_pet(m_ptr)) check_pets_num_and_align(m_ptr, FALSE);

	m_ptr->r_idx = r_idx;
	m_ptr->ap_r_idx = r_idx;
	update_mon(m_idx, FALSE);
	lite_spot(m_ptr->fy, m_ptr->fx);

	if ((r_info[old_r_idx].flags7 & (RF7_LITE_MASK | RF7_DARK_MASK)) ||
	    (r_ptr->flags7 & (RF7_LITE_MASK | RF7_DARK_MASK)))
		p_ptr->update |= (PU_MON_LITE);

	if (is_pet(m_ptr)) check_pets_num_and_align(m_ptr, TRUE);

	if (born)
	{
		/* Sub-alignment of a chameleon */
		if (r_ptr->flags3 & (RF3_ANG_CHAOS | RF3_ANG_COSMOS))
		{
			m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
			if (r_ptr->flags3 & RF3_ANG_CHAOS) m_ptr->sub_align |= SUB_ALIGN_EVIL;
			if (r_ptr->flags3 & RF3_ANG_COSMOS) m_ptr->sub_align |= SUB_ALIGN_GOOD;
		}
		return;
	}

	if (m_idx == p_ptr->riding)
	{
		char m_name[80];
		monster_desc(m_name, m_ptr, 0);
#ifdef JP
		msg_format("突然%sが変身した。", old_m_name);
#else
		msg_format("Suddenly, %s transforms!", old_m_name);
#endif
		if (!(r_ptr->flags7 & RF7_RIDING))
#ifdef JP
			if (rakuba(0, TRUE)) msg_print("地面に落とされた。");
#else
			if (rakuba(0, TRUE)) msg_format("You have fallen from %s.", m_name);
#endif
	}

	/*:::加速、MAXHP,MAX_MAXHPの計算　悪夢モード有り
     *:::MAXHPが減っている場合それに応じて新たなモンスターのMAXHPも減る　それ以外は全快らしい*/
///system HPの変数変える場合ここも変える　変身前のHPの減りに応じて変身先のHPも減るようにしておこうか
	/* Extract the monster base speed */
	m_ptr->mspeed = get_mspeed(r_ptr);

	oldmaxhp = m_ptr->max_maxhp;
	/* Assign maximal hitpoints */
	if (r_ptr->flags1 & RF1_FORCE_MAXHP)
	{
		m_ptr->max_maxhp = maxroll(r_ptr->hdice, r_ptr->hside);
	}
	else
	{
		m_ptr->max_maxhp = damroll(r_ptr->hdice, r_ptr->hside);
	}

	/* Monsters have double hitpoints in Nightmare mode */
	if (ironman_nightmare)
	{
		u32b hp = m_ptr->max_maxhp * 2L;
		m_ptr->max_maxhp = (s16b)MIN(30000, hp);
	}
	else if(difficulty == DIFFICULTY_EASY) m_ptr->max_maxhp = MAX(1,m_ptr->max_maxhp/2);
	else if(difficulty == DIFFICULTY_NORMAL) m_ptr->max_maxhp = MAX(1,(m_ptr->max_maxhp/2 + m_ptr->max_maxhp/4));

	m_ptr->maxhp = (long)(m_ptr->maxhp * m_ptr->max_maxhp) / oldmaxhp;
	if (m_ptr->maxhp < 1) m_ptr->maxhp = 1;
	m_ptr->hp = (long)(m_ptr->hp * m_ptr->max_maxhp) / oldmaxhp;
}


/*
 *  Hook for Tanuki
 */
/*:::狸の見た目のモンスターを決めるルーチン*/
///mod140625 QUESTORと特殊モンスター以外の何にでも変身するようにしてみた
static bool monster_hook_tanuki(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	//if (r_ptr->flags1 & (RF1_UNIQUE)) return FALSE;
	//if (r_ptr->flags2 & RF2_MULTIPLY) return FALSE;
	//if (r_ptr->flags7 & (RF7_FRIENDLY | RF7_CHAMELEON)) return FALSE;
	//if (r_ptr->flags7 & RF7_AQUATIC) return FALSE;

	//if ((r_ptr->blow[0].method == RBM_EXPLODE) || (r_ptr->blow[1].method == RBM_EXPLODE) || (r_ptr->blow[2].method == RBM_EXPLODE) || (r_ptr->blow[3].method == RBM_EXPLODE))
	//	return FALSE;

	if (r_ptr->flags7 & (RF7_VARIABLE )) return FALSE;



	if(r_ptr->flags1 & (RF1_QUESTOR)) return FALSE;
	if(r_ptr->flags7 & (RF7_TANUKI | RF7_CHAMELEON)) return FALSE;
	///mod141126 霊夢と魔理沙には変身しないようにした。関係ないかもしれないがバグ対策
	if(r_ptr->flags7 & RF7_VARIABLE ) return FALSE;

	//v1.1.51 新アリーナ内では東方ユニークだけにする
	if (p_ptr->inside_arena) return is_gen_unique(r_idx);

	///mod150627 inside_battle追加
	if( p_ptr->inside_battle) return TRUE;

	return (*(get_monster_hook()))(r_idx);
}


/*
 *  Set initial racial appearance of a monster
 */
/*:::モンスターの見た目を変える　たぬき限定処理らしい　混沌とかドワーキンは描画時に変えているのか？*/
static int initial_r_appearance(int r_idx)
{
	int attempts = 1000;

	int ap_r_idx;
	int min = MIN(base_level-5, 50);

	if (!(r_info[r_idx].flags7 & RF7_TANUKI))
		return r_idx;

	get_mon_num_prep(monster_hook_tanuki, NULL);

	while (--attempts)
	{
		ap_r_idx = get_mon_num(base_level + 10);
		if (r_info[ap_r_idx].level >= min) return ap_r_idx;
	}

	return r_idx;
}


/*
 * Get initial monster speed
 */
/*:::モンスターの加速を計算。ユニークとアリーナ以外ではややばらつく*/
///mod140721 UNIQUE2もばらつかないようにした。
//映姫要例外処理追加
byte get_mspeed(monster_race *r_ptr)
{
	/* Extract the monster base speed */
	int mspeed = r_ptr->speed;

	/* Hack -- small racial variety */
	if (!(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2) && !p_ptr->inside_arena && p_ptr->pclass != CLASS_EIKI)
	{
		/* Allow some small variation per monster */
		int i = SPEED_TO_ENERGY(r_ptr->speed) / (one_in_(4) ? 3 : 10);
		if (i) mspeed += rand_spread(0, i);
	}

	if (mspeed > 199) mspeed = 199;

	return (byte)mspeed;
}


/*:::赤蛮奇の頭のパラメータ設定をする。＠のMAXレベルに依存する。*/
bool apply_mon_race_banki_head(int r_idx)
{
	int lev;
	monster_race	*r_ptr = &r_info[r_idx];

	if(r_idx != MON_BANKI_HEAD_1 && r_idx != MON_BANKI_HEAD_2)
	{
		msg_format(_("ERROR:赤蛮奇の頭のパラメータ設定ルーチンが未登録のr_idx(%d)で呼ばれた", "ERROR:apply_mon_race_banki_head() called with invalid r_idx, %d"),r_idx);
		return FALSE;
	}

	lev = p_ptr->max_plv;
	r_ptr->level = lev;

	r_ptr->hdice = lev ;
	r_ptr->hside = 10 ;

	r_ptr->ac = lev * 2 + 20;
	r_ptr->aaf = 100;

	if(lev > 34)
	{
		if(r_idx == MON_BANKI_HEAD_1) r_ptr->speed = 100 + lev / 2;
		else r_ptr->speed = 95 + lev / 2;
	}
	r_ptr->mexp = lev;

	//頭2は遠隔攻撃専用なのでここで終わり
	if(r_idx == MON_BANKI_HEAD_2) return TRUE;

	r_ptr->blow[0].method = RBM_CRUSH;
	r_ptr->blow[0].effect = RBE_HURT;
	r_ptr->blow[0].d_dice = MAX(1,lev/5);
	r_ptr->blow[0].d_side = MAX(1,lev/5);

	if(lev > 19)
	{
		r_ptr->blow[1].method = RBM_GAZE;
		r_ptr->blow[1].effect = RBE_TERRIFY;
		r_ptr->blow[1].d_dice = MAX(1,lev/6);
		r_ptr->blow[1].d_side = MAX(1,lev/6);
	}
	else r_ptr->blow[1].method = 0;
	if(lev > 29)
	{
		r_ptr->blow[2].method = RBM_CRUSH;
		r_ptr->blow[2].effect = RBE_HURT;
		r_ptr->blow[2].d_dice = MAX(1,lev/7);
		r_ptr->blow[2].d_side = MAX(1,lev/7);
	}
	else r_ptr->blow[2].method = 0;
	if(lev > 39)
	{
		r_ptr->blow[3].method = RBM_GAZE;
		r_ptr->blow[3].effect = RBE_CONFUSE;
		r_ptr->blow[3].d_dice = MAX(1,lev/8);
		r_ptr->blow[3].d_side = MAX(1,lev/8);
	}
	else r_ptr->blow[3].method = 0;


	return TRUE;
}

/*:::グリーンアイドモンスターのパラメータ設定をする。敵のレベルに依存する。*/
bool apply_mon_race_green_eyed(void)
{
	int lev;
	int target_m_idx = p_ptr->magic_num1[0];
	monster_race	*r_ptr = &r_info[MON_GREEN_EYED];
	monster_race	*target_r_ptr;

	if(!m_list[target_m_idx].r_idx)
	{
		msg_format(_("ERROR:グリーンアイドモンスターのターゲットM_IDXがおかしい(%d)", "ERROR:invalid target_m_idx for green-eyed monster, %d"), target_m_idx);
		return FALSE;
	}
	target_r_ptr = &r_info[m_list[target_m_idx].r_idx];
	lev = target_r_ptr->level;

	if(target_r_ptr->flags1 & RF1_FEMALE)
		lev = lev * 4 / 3;
	if(lev < 1) lev = 1;
	else if(lev > 127) lev = 127;


	r_ptr->level = lev;

	r_ptr->hdice = lev ;
	r_ptr->hside = (lev+30)/3;

	r_ptr->ac = lev;
	r_ptr->aaf = 100;

	if(lev > 29) r_ptr->speed = 105 + (lev-30) / 4;
	r_ptr->mexp = 1;

	r_ptr->blow[0].method = RBM_GAZE;
	r_ptr->blow[0].effect = RBE_TERRIFY;
	r_ptr->blow[0].d_dice = MAX(1,lev/20);
	r_ptr->blow[0].d_side = MAX(1,lev/5);

	if(lev > 19)
	{
		r_ptr->blow[1].method = RBM_CRUSH;
		r_ptr->blow[1].effect = RBE_HURT;
		r_ptr->blow[1].d_dice = MAX(1,(lev/10+2));
		r_ptr->blow[1].d_side = MAX(1,(lev/12+2));
	}
	else r_ptr->blow[1].method = 0;
	if(lev > 49)
	{
		r_ptr->blow[2].method = RBM_GAZE;
		r_ptr->blow[2].effect = RBE_CONFUSE;
		r_ptr->blow[2].d_dice = MAX(1,lev/7);
		r_ptr->blow[2].d_side = MAX(1,lev/7);
	}
	else r_ptr->blow[2].method = 0;
	if(lev > 79)
	{
		r_ptr->blow[3].method = RBM_CRUSH;
		r_ptr->blow[3].effect = RBE_STUN;
		r_ptr->blow[3].d_dice = MAX(1,lev/16);
		r_ptr->blow[3].d_side = MAX(1,lev/3);
	}
	else r_ptr->blow[3].method = 0;

	if(lev > 19)  r_ptr->flagsr |= RFR_IM_ACID;
	if(lev > 29)  r_ptr->flagsr |= RFR_IM_FIRE;
	if(lev > 39)  r_ptr->flagsr |= RFR_IM_COLD;
	if(lev > 49)  r_ptr->flagsr |= RFR_IM_ELEC;
	if(lev > 59)  r_ptr->flagsr |= RFR_IM_POIS;

	return TRUE;
}


/*:::輝夜のパラメータ設定をする。プレイヤーのMAXレベルに依存する。*/
bool apply_mon_race_master_kaguya(void)
{
	int lev;
	monster_race	*r_ptr = &r_info[MON_MASTER_KAGUYA];

	lev = p_ptr->max_plv;

	r_ptr->level = lev;

	//HP Lev1:60 Lev10:200 Lev20:700 Lev30:1500 Lev40:2600 Lev50:4000
	r_ptr->hdice = lev * 3 / 2 + 5;
	r_ptr->hside = MAX(lev,10) ;

	r_ptr->ac = lev * 3 + 10;
	r_ptr->aaf = 50;

	if(lev < 30) r_ptr->speed = 110;
	else r_ptr->speed = 110 + (lev - 25);

	r_ptr->mexp = 1;

	if(lev < 30) r_ptr->freq_spell = 25;
	else if(lev < 40) r_ptr->freq_spell = 33;
	else r_ptr->freq_spell = 50;

	r_ptr->blow[0].method = RBM_STRIKE;
	r_ptr->blow[0].effect = RBE_HURT;
	r_ptr->blow[0].d_dice = MAX(1,lev/5);
	r_ptr->blow[0].d_side = 5 + MAX(1,lev/10);

	if(lev > 9)
	{
		r_ptr->blow[1].method = RBM_FLASH;
		r_ptr->blow[1].effect = RBE_ELEMENT;
		r_ptr->blow[1].d_dice = 4 + MAX(1,lev/4);
		r_ptr->blow[1].d_side = 4 + MAX(1,lev/4);
	}
	else r_ptr->blow[1].method = 0;

	if(lev > 24)
	{
		r_ptr->blow[2].method = RBM_PRESS;
		r_ptr->blow[2].effect = RBE_STUN;
		r_ptr->blow[2].d_dice = 12;
		r_ptr->blow[2].d_side = MAX(1,lev/3);
	}
	else r_ptr->blow[2].method = 0;

	if(lev > 39)
	{
		r_ptr->blow[3].method = RBM_PRESS;
		r_ptr->blow[3].effect = RBE_SUPERHURT;
		r_ptr->blow[3].d_dice = 16;
		r_ptr->blow[3].d_side = 4 + MAX(1,lev/3);
	}
	else r_ptr->blow[3].method = 0;


	r_ptr->flags1 = (RF1_FEMALE | RF1_FORCE_MAXHP | RF1_ATTR_MULTI);

	r_ptr->flags2 = (RF2_OPEN_DOOR | RF2_REGENERATE);
	if(lev > 29) r_ptr->flags2 |= RF2_SMART;
	if(lev > 39) r_ptr->flags2 |= RF2_AURA_FIRE | RF2_AURA_ELEC | RF2_AURA_COLD;
	if(lev > 44) r_ptr->flags2 |= RF2_POWERFUL;

	r_ptr->flags3 = (RF3_HUMAN | RF3_GEN_FRIENDLY);
	if(lev > 29) r_ptr->flags3 |= RF3_NO_CONF;
	if(lev > 34) r_ptr->flags3 |= RF3_NO_STUN;
	if(lev > 39) r_ptr->flags3 |= RF3_NO_SLEEP;

	r_ptr->flags4 = (RF4_DANMAKU);
	if(lev > 34) r_ptr->flags4 |= RF4_DISPEL;

	r_ptr->flags5 = 0L;
	if(lev > 19 && lev < 40) r_ptr->flags5 |= (RF5_BA_ACID | RF5_BA_ELEC | RF5_BA_FIRE | RF5_BA_COLD);
	if(lev > 9  && lev < 20) r_ptr->flags5 |= (RF5_BO_ACID | RF5_BO_ELEC | RF5_BO_FIRE | RF5_BO_COLD);
	if(lev > 29 && lev < 45) r_ptr->flags5 |= RF5_BA_WATE;
	if(lev > 14 && lev < 30) r_ptr->flags5 |= RF5_BO_WATE;
	if(lev > 47) r_ptr->flags5 |= RF5_BA_MANA;
	if(lev > 24 && lev < 40) r_ptr->flags5 |= RF5_BO_MANA;
	if(lev > 39) r_ptr->flags5 |= (RF5_BA_LITE | RF5_BA_DARK);

	r_ptr->flags6 = 0L;
	if(lev >  9) r_ptr->flags6 |= RF6_BLINK;
	if(lev > 19) r_ptr->flags6 |= RF6_TPORT;
	if(lev > 29) r_ptr->flags6 |= RF6_HEAL;
	if(lev > 39) r_ptr->flags6 |= RF6_HASTE;
	if(lev > 49) r_ptr->flags6 |= RF6_INVULNER;

	r_ptr->flags7 = RF7_FRIENDLY | RF7_UNIQUE2 | RF7_VARIABLE;
	if(lev > 9) r_ptr->flags7 |= RF7_HAS_LITE_1;
	if(lev > 24) r_ptr->flags7 |= RF7_CAN_FLY;

	r_ptr->flags9 = 0L;
	if(lev > 39) r_ptr->flags9 |= (RF9_FIRE_STORM  | RF9_ICE_STORM | RF9_THUNDER_STORM | RF9_ACID_STORM);
	if(lev > 42) r_ptr->flags9 |= RF9_BA_METEOR;
	if(lev > 29) r_ptr->flags9 |= RF9_BEAM_LITE;
	if(lev > 39) r_ptr->flags9 |= RF9_BA_DISI;
	if(lev > 34) r_ptr->flags9 |= RF9_TELE_HI_APPROACH;

	r_ptr->flagsr = (RFR_RES_HOLY);
	if(lev > 19)  r_ptr->flagsr |= RFR_IM_ACID;
	if(lev > 19)  r_ptr->flagsr |= RFR_IM_ELEC;
	if(lev > 19)  r_ptr->flagsr |= RFR_IM_FIRE;
	if(lev > 19)  r_ptr->flagsr |= RFR_IM_COLD;
	if(lev > 29)  r_ptr->flagsr |= RFR_IM_POIS;
	if(lev > 34)  r_ptr->flagsr |= RFR_RES_LITE;
	if(lev > 34)  r_ptr->flagsr |= RFR_RES_DARK;
	if(lev > 39)  r_ptr->flagsr |= RFR_RES_DISE;
	if(lev > 44)  r_ptr->flagsr |= RFR_RES_NETH;
	if(lev > 39)  r_ptr->flagsr |= RFR_RES_CHAO;
	if(lev > 29)  r_ptr->flagsr |= RFR_RES_TIME;

	return TRUE;
}


/*:::霊夢（敵）のパラメータ設定をする。現在のダンジョンレベルに依存する。*/
bool apply_mon_race_reimu(void)
{
	int lev;
	monster_race	*r_ptr = &r_info[MON_REIMU];

	if(p_ptr->inside_battle) lev = battle_mon_base_level;
	else if (p_ptr->inside_arena) lev = nightmare_mon_base_level;
	else if(dun_level) lev = dun_level;
	else lev = wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].level;
	if(lev==0) lev = 1;
	if(lev < 1 || lev > 128)
	{
		msg_print(_("ERROR:霊夢のレベルがおかしい", "ERROR:Reimu's level is strange"));
		return FALSE;
	}
	r_ptr->level = lev;

	r_ptr->hdice = MAX(r_ptr->level *4/3 ,8) ;
	r_ptr->hside = MAX(r_ptr->level,10) ;

	r_ptr->ac = lev * 3 / 2 + 30;
	r_ptr->aaf = MIN(100,lev+30);

	if(lev < 29) r_ptr->speed = 110;
	else r_ptr->speed = 110 + lev / 5;
	r_ptr->mexp = lev * lev * lev / 10 + 100;

	if(lev < 50) r_ptr->freq_spell = 25;
	else if(lev < 100) r_ptr->freq_spell = 33;
	else r_ptr->freq_spell = 50;

	r_ptr->blow[0].method = RBM_STRIKE;
	if(lev > 49) r_ptr->blow[0].effect = RBE_SMITE;
	else r_ptr->blow[0].effect = RBE_HURT;
	r_ptr->blow[0].d_dice = MAX(1,lev/8);
	r_ptr->blow[0].d_side = 5 + MAX(1,lev/10);

	if(lev > 9)
	{
		r_ptr->blow[1].method = RBM_KICK;
		if(lev > 59) r_ptr->blow[1].effect = RBE_STUN;
		else r_ptr->blow[1].effect = RBE_HURT;
		r_ptr->blow[1].d_dice = 2 + MAX(1,lev/12);
		r_ptr->blow[1].d_side = 2 + MAX(1,lev/12);
	}
	else r_ptr->blow[1].method = 0;
	if(lev > 24)
	{
		r_ptr->blow[2].method = RBM_STRIKE;
		if(lev > 69) r_ptr->blow[2].effect = RBE_SMITE;
		else r_ptr->blow[2].effect = RBE_HURT;
		r_ptr->blow[2].d_dice = MAX(1,lev/8);
		r_ptr->blow[2].d_side = 5 + MAX(1,lev/10);
	}
	else r_ptr->blow[2].method = 0;
	if(lev > 39)
	{
		r_ptr->blow[3].method = RBM_KICK;
		if(lev > 79) r_ptr->blow[3].effect = RBE_SUPERHURT;
		else r_ptr->blow[3].effect = RBE_HURT;
		r_ptr->blow[3].d_dice = 5 + MAX(1,lev/10);
		r_ptr->blow[3].d_side = 5 + MAX(1,lev/10);
	}
	else r_ptr->blow[3].method = 0;

	r_ptr->flags1 = (RF1_FEMALE | RF1_FORCE_MAXHP | RF1_FORCE_SLEEP);
	r_ptr->flags1 |= RF1_DROP_1D2;
	if(lev > 29 && lev < 60 || lev > 89) r_ptr->flags1 |= RF1_DROP_2D2;
	if(lev > 59) r_ptr->flags1 |= RF1_DROP_3D2;
	if(lev > 100) r_ptr->flags1 |= RF1_DROP_4D2;
	if(lev > 29) r_ptr->flags1 |= RF1_DROP_GOOD;
	if(lev > 29) r_ptr->flags1 |= RF1_ONLY_ITEM;
	if(lev > 79) r_ptr->flags1 |= RF1_DROP_GREAT;

	r_ptr->flags2 = (RF2_OPEN_DOOR | RF2_BASH_DOOR);
	if(lev > 29) r_ptr->flags2 |= RF2_SMART;
	if(lev > 29) r_ptr->flags2 |= RF2_REGENERATE;
	if(lev > 49) r_ptr->flags2 |= RF2_POWERFUL;
	if(lev > 49) r_ptr->flags2 |= RF2_KILL_BODY;
	if(lev > 69) r_ptr->flags2 |= RF2_REFLECTING;
	if(lev > 89) r_ptr->flags2 |= RF2_PASS_WALL;

	r_ptr->flags3 = (RF3_HUMAN | RF3_GEN_WARLIKE);
	if(lev > 19) r_ptr->flags3 |= RF3_NO_SLEEP;
	if(lev > 29) r_ptr->flags3 |= RF3_NO_FEAR;
	if(lev > 39) r_ptr->flags3 |= RF3_NO_CONF;
	if(lev > 49) r_ptr->flags3 |= RF3_NO_STUN;

	r_ptr->flags4 = (RF4_DANMAKU);
	if(lev > 49) r_ptr->flags4 |= RF4_DISPEL;

	r_ptr->flags5 = 0L;
	if(lev > 100) r_ptr->flags5 |= RF5_BA_MANA;
	if(lev > 44) r_ptr->flags5 |= RF5_BA_LITE;
	if(lev < 20) r_ptr->flags5 |= RF5_MISSILE;
	if(lev > 34 && lev < 100) r_ptr->flags5 |= RF5_BO_MANA;
	if(lev > 29 && lev < 80) r_ptr->flags5 |= RF5_HOLD;

	r_ptr->flags6 = 0L;
	if(lev > 89) r_ptr->flags6 |= RF6_HASTE;
	if(lev > 39) r_ptr->flags6 |= RF6_HEAL;
	if(lev > 79) r_ptr->flags6 |= RF6_INVULNER;
	if(lev > 9) r_ptr->flags6 |= RF6_BLINK;
	if(lev > 84) r_ptr->flags6 |= RF6_PSY_SPEAR;
	if(lev > 39) r_ptr->flags6 |= RF6_SPECIAL;//夢想封印

	r_ptr->flags9 = 0L;
	///mod150718 原子分解→夢想封印コンボが危険過ぎるので廃止
	//if(lev > 89) r_ptr->flags9 |= RF9_BA_DISI;
	if(lev > 29 && lev < 70) r_ptr->flags9 |= RF9_BO_HOLY;
	if(lev > 49 && lev < 90) r_ptr->flags9 |= RF9_BA_HOLY;
	if(lev > 89) r_ptr->flags9 |= RF9_HOLYFIRE;
	if(lev > 79) r_ptr->flags9 |= RF9_S_DEITY;
	if(lev > 64) r_ptr->flags9 |= RF9_TELE_HI_APPROACH;
	if(lev < 25 ) r_ptr->flags9 |= RF9_PUNISH_1;
	if(lev > 14 && lev < 45 ) r_ptr->flags9 |= RF9_PUNISH_2;
	if(lev > 29 && lev < 60 ) r_ptr->flags9 |= RF9_PUNISH_3;
	if(lev > 44 && lev < 90) r_ptr->flags9 |= RF9_PUNISH_4;

	r_ptr->flagsr = (RFR_RES_HOLY);
	if(lev > 49)  r_ptr->flagsr |= RFR_IM_ACID;
	if(lev > 39)  r_ptr->flagsr |= RFR_IM_ELEC;
	if(lev > 19)  r_ptr->flagsr |= RFR_IM_FIRE;
	if(lev > 19)  r_ptr->flagsr |= RFR_IM_COLD;
	if(lev > 69)  r_ptr->flagsr |= RFR_IM_POIS;
	if(lev > 34)  r_ptr->flagsr |= RFR_RES_LITE;
	if(lev > 74)  r_ptr->flagsr |= RFR_RES_DARK;
	if(lev > 84)  r_ptr->flagsr |= RFR_RES_NETH;
	if(lev > 49)  r_ptr->flagsr |= RFR_RES_WATE;
	if(lev > 49)  r_ptr->flagsr |= RFR_RES_SOUN;
	if(lev > 59)  r_ptr->flagsr |= RFR_RES_CHAO;
	if(lev > 89)  r_ptr->flagsr |= RFR_RES_TIME;
	if(lev > 49)  r_ptr->flagsr |= RFR_RES_TELE;

	return TRUE;
}


/*:::魔理沙（敵）のパラメータ設定をする。現在のダンジョンレベルに依存する。*/
bool apply_mon_race_marisa(void)
{
	int lev;
	monster_race	*r_ptr = &r_info[MON_MARISA];

	if(p_ptr->inside_battle) lev = battle_mon_base_level;
	else if (p_ptr->inside_arena) lev = nightmare_mon_base_level;
	else if(dun_level) lev = dun_level;
	else lev = wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].level;


	if(lev==0) lev = 1;
	if(lev < 1 || lev > 128)
	{
		msg_print(_("ERROR:魔理沙のレベルがおかしい", "ERROR:Marisa's level is strange"));
		return FALSE;
	}
	r_ptr->level = lev;


	r_ptr->hdice = MAX(lev,10);
	r_ptr->hside = MAX(lev,10);
	r_ptr->ac = lev + 40;
	r_ptr->aaf = MIN(100,lev+50);
	if(lev > 19) r_ptr->speed = 120 + lev / 5;
	else r_ptr->speed = 115;
	r_ptr->mexp = lev * lev * lev / 10 + 100;

	if(lev < 40) r_ptr->freq_spell = 25;
	else if(lev < 70) r_ptr->freq_spell = 33;
	else r_ptr->freq_spell = 50;

	r_ptr->blow[0].method = RBM_STRIKE;
	if(lev > 49) r_ptr->blow[0].effect = RBE_SUPERHURT;
	else r_ptr->blow[0].effect = RBE_HURT;
	r_ptr->blow[0].d_dice = MAX(1,lev/10);
	r_ptr->blow[0].d_side = 4 + MAX(1,lev/16);

	if(lev > 4)
	{
		r_ptr->blow[1].method = RBM_TOUCH;
		if(lev < 20) r_ptr->blow[1].effect = RBE_EAT_GOLD;
		else r_ptr->blow[1].effect = RBE_EAT_ITEM;
		r_ptr->blow[1].d_dice = 0;
		r_ptr->blow[1].d_side = 0;
	}
	if(lev > 24)
	{
		r_ptr->blow[2].method = RBM_STRIKE;
		if(lev > 69) r_ptr->blow[2].effect = RBE_STUN;
		else r_ptr->blow[2].effect = RBE_HURT;
		r_ptr->blow[2].d_dice = MAX(1,lev/10);
		r_ptr->blow[2].d_side = 3 + MAX(1,lev/10);
	}
	if(lev > 44)
	{
		r_ptr->blow[3].method = RBM_CRUSH;
		r_ptr->blow[3].effect = RBE_FIRE;
		r_ptr->blow[3].d_dice = lev / 3;
		r_ptr->blow[3].d_side = lev / 3;
	}

	r_ptr->flags1 = (RF1_FEMALE | RF1_FORCE_MAXHP | RF1_FORCE_SLEEP);
	r_ptr->flags1 |= RF1_DROP_1D2;
	if(lev > 29 && lev < 80 || lev > 100) r_ptr->flags1 |= RF1_DROP_2D2;
	if(lev > 49) r_ptr->flags1 |= RF1_DROP_3D2;
	if(lev > 79) r_ptr->flags1 |= RF1_DROP_4D2;
	r_ptr->flags1 |= RF1_ONLY_ITEM;
	r_ptr->flags1 |= RF1_DROP_GOOD;
	if(lev > 69) r_ptr->flags1 |= RF1_DROP_GREAT;

	r_ptr->flags2 = (RF2_OPEN_DOOR | RF2_BASH_DOOR);
	if(lev > 29) r_ptr->flags2 |= RF2_SMART;
	if(lev > 19 && (lev % 7 == 2 || lev % 7 == 1))  r_ptr->flags2 |= RF2_INVISIBLE;
	if(lev > 69) r_ptr->flags2 |= RF2_POWERFUL;
	if(lev > 29) r_ptr->flags2 |= RF2_MOVE_BODY;
	if(lev > 49 && (lev % 5 == 2)) r_ptr->flags2 |= RF2_AURA_FIRE;
	if(lev > 49 && (lev % 5 == 1)) r_ptr->flags2 |= RF2_AURA_ELEC;
	if(lev > 49 && (lev % 5 == 3)) r_ptr->flags2 |= RF2_AURA_COLD;
	if(lev > 59 && (lev % 7 == 3)) r_ptr->flags2 |= RF2_TELE_AFTER;

	r_ptr->flags3 = (RF3_HUMAN | RF3_GEN_WARLIKE);
	if(lev > 29) r_ptr->flags3 |= RF3_NO_SLEEP;
	if(lev > 59) r_ptr->flags3 |= RF3_NO_FEAR;
	if(lev > 49) r_ptr->flags3 |= RF3_NO_CONF;
	if(lev > 79) r_ptr->flags3 |= RF3_NO_STUN;

	r_ptr->flags4 |= RF4_DANMAKU;
	if(lev > 39 && (lev % 7 == 3)) r_ptr->flags4 |= RF4_BA_CHAO;
	if( (lev % 7 == 5)) r_ptr->flags4 |= RF4_DISPEL;

/*
lev%7 ==
0:地獄、アンデッド
1:水、原子分解、竜巻
2:妨害、精神、暗黒、悪魔
3:カオス、テレポ、汚染、空間歪曲
4:呪い系、ヘルファイア
5:光の剣、無敵、治癒、魔力消去
6:召喚
*/
	r_ptr->flags5 = 0L;
	if(lev > 19 && lev < 50 && (lev % 5 == 0))  r_ptr->flags5 |= RF5_BA_ACID;
	if(lev > 19 && lev < 50 && (lev % 5 == 1))  r_ptr->flags5 |= RF5_BA_ELEC;
	if(lev > 19 && lev < 50 && (lev % 5 == 2))  r_ptr->flags5 |= RF5_BA_FIRE;
	if(lev > 19 && lev < 50 && (lev % 5 == 3))  r_ptr->flags5 |= RF5_BA_COLD;
	if(lev > 19 && lev < 50 && (lev % 5 == 4))  r_ptr->flags5 |= RF5_BA_POIS;

	if(lev > 19 && lev < 50 && (lev % 7 == 2))  r_ptr->flags5 |= RF5_SCARE;
	if(lev > 19 && lev < 50 && (lev % 7 == 2))  r_ptr->flags5 |= RF5_BLIND;
	if(lev > 19 && lev < 50 && (lev % 7 == 2))  r_ptr->flags5 |= RF5_CONF;
	if(lev > 19 && lev < 50 && (lev % 7 == 2))  r_ptr->flags5 |= RF5_SLOW;
	if(lev > 24 && lev < 50 && (lev % 7 == 2))  r_ptr->flags5 |= RF5_HOLD;

	if(lev > 4 && lev < 30 && (lev % 5 == 0))  r_ptr->flags5 |= RF5_BO_ACID;
	if(lev > 4 && lev < 30 && (lev % 5 == 1))  r_ptr->flags5 |= RF5_BO_ELEC;
	if(lev > 4 && lev < 30 && (lev % 5 == 2))  r_ptr->flags5 |= RF5_BO_FIRE;
	if(lev > 4 && lev < 30 && (lev % 5 == 3))  r_ptr->flags5 |= RF5_BO_COLD;

	if(lev > 39 && lev < 80 && (lev % 7 == 0))  r_ptr->flags5 |= RF5_BA_NETH;
	if(lev > 39 && lev < 80 && (lev % 7 == 1))  r_ptr->flags5 |= RF5_BA_WATE;
	if(lev > 39 && (lev % 7 == 2))  r_ptr->flags5 |= RF5_BA_DARK;

	if(lev > 24 && lev < 60 && (lev % 7 == 0))  r_ptr->flags5 |= RF5_BO_NETH;
	if(lev > 24 && lev < 60 && (lev % 7 == 1))  r_ptr->flags5 |= RF5_BO_WATE;
	if(lev > 24 && lev < 60 && (lev % 5 == 3))  r_ptr->flags5 |= RF5_BO_ICEE;

	if(lev < 25 )  r_ptr->flags5 |= RF5_MISSILE;
	if(lev > 24 && lev < 80 && (lev % 3 == 0))  r_ptr->flags5 |= RF5_BO_MANA;
	if(lev > 24 && lev < 80 && (lev % 3 == 1))  r_ptr->flags5 |= RF5_BO_PLAS;

	if(lev > 39)  r_ptr->flags5 |= RF5_BA_LITE;
	if(lev > 100)  r_ptr->flags5 |= RF5_BA_MANA;

	if(lev > 9 && lev < 25 && (lev % 7 == 4))  r_ptr->flags5 |= RF5_CAUSE_1;
	if(lev > 19 && lev < 40 && (lev % 7 == 4))  r_ptr->flags5 |= RF5_CAUSE_2;
	if(lev > 29 && lev < 70 && (lev % 7 == 4))  r_ptr->flags5 |= RF5_CAUSE_3;
	if(lev > 49 && lev < 90 && (lev % 7 == 4))  r_ptr->flags5 |= RF5_CAUSE_4;

	if(lev > 29 && lev < 50 && (lev % 7 == 2))  r_ptr->flags5 |= RF5_MIND_BLAST;
	if(lev > 49 && (lev % 7 == 2))  r_ptr->flags5 |= RF5_BRAIN_SMASH;
	r_ptr->flags6 = 0L;
	if(lev > 49) r_ptr->flags6 |= RF6_HASTE;
	if(lev > 69 && (lev%7 == 4)) r_ptr->flags6 |= RF6_HAND_DOOM;
	if(lev > 79 && (lev%7 == 5)) r_ptr->flags6 |= RF6_INVULNER;
	if(lev > 9) r_ptr->flags6 |= RF6_BLINK;
	if(lev > 29) r_ptr->flags6 |= RF6_TPORT;
	if(lev > 29 && (lev % 7 == 3)) r_ptr->flags6 |= RF6_TELE_TO;
	if(lev > 39 && (lev % 7 == 3)) r_ptr->flags6 |= RF6_TELE_AWAY;
	if(lev > 59 && (lev%7 == 5)) r_ptr->flags6 |= RF6_PSY_SPEAR;
	if(lev > 49 && (lev%7 == 5)) r_ptr->flags6 |= RF6_HEAL;
	if(lev > 9 && (lev % 7 == 2)) r_ptr->flags6 |= RF6_TRAPS;
	if(lev > 19 && (lev % 7 == 2)) r_ptr->flags6 |= RF6_FORGET;

	if(lev > 89 && (lev%7 == 3)) r_ptr->flags6 |= RF6_S_CYBER;
	if(lev > 49 && (lev%7 == 6)) r_ptr->flags6 |= RF6_S_MONSTERS;
	if(lev > 24 && lev < 70 && (lev % 7 == 6))  r_ptr->flags6 |= RF6_S_MONSTER;
	if(lev < 35 && (lev % 7 == 6))  r_ptr->flags6 |= RF6_S_ANT;
	if(lev < 35 && (lev % 7 == 6))  r_ptr->flags6 |= RF6_S_SPIDER;
	if(lev > 34 && (lev % 7 == 6))  r_ptr->flags6 |= RF6_S_HOUND;
	if(lev > 54 && (lev % 7 == 6))  r_ptr->flags6 |= RF6_S_HYDRA;
	if(lev > 39 && (lev % 7 == 2))  r_ptr->flags6 |= RF6_S_DEMON;
	if(lev > 49 && (lev % 7 == 6))  r_ptr->flags6 |= RF6_S_DRAGON;
	if(lev > 79 && (lev % 7 == 6))  r_ptr->flags6 |= RF6_S_HI_DRAGON;

	if(lev > 19 && (lev % 7 == 0))  r_ptr->flags6 |= RF6_S_UNDEAD;
	if(lev > 59 && (lev % 7 == 0))  r_ptr->flags6 |= RF6_S_HI_UNDEAD;

	if(p_ptr->inside_quest == QUEST_SATORI) r_ptr->flags6 |= RF6_S_KIN; //特殊処理　さとりクエストのときゴーレム召喚

	r_ptr->flags9 = 0L;
	if(lev > 39 && (lev % 5 == 0))  r_ptr->flags9 |= RF9_ACID_STORM;
	if(lev > 39 && (lev % 5 == 1))  r_ptr->flags9 |= RF9_THUNDER_STORM;
	if(lev > 39 && (lev % 5 == 2))  r_ptr->flags9 |= RF9_FIRE_STORM;
	if(lev > 39 && (lev % 5 == 3))  r_ptr->flags9 |= RF9_ICE_STORM;
	if(lev > 39 && (lev % 5 == 4))  r_ptr->flags9 |= RF9_TOXIC_STORM;
	if(lev > 49 && (lev % 7 == 3))  r_ptr->flags9 |= RF9_BA_POLLUTE;
	if(lev > 49 && (lev % 7 == 1))  r_ptr->flags9 |= RF9_BA_DISI;

	if(lev > 89) r_ptr->flags9 |= RF9_BA_METEOR;
	if(lev > 79 && (lev % 7 == 3))  r_ptr->flags9 |= RF9_BA_DISTORTION;
	if(lev > 49) r_ptr->flags9 |= RF9_GIGANTIC_LASER;
	if(lev > 19 && (lev % 7 == 6))  r_ptr->flags9 |= RF9_S_ANIMAL;
	if(lev > 79 )  r_ptr->flags9 |= RF9_FINALSPARK;
	if(lev > 29 && (lev % 7 == 1))  r_ptr->flags9 |= RF9_TORNADO;
	if(lev > 79 && (lev % 7 == 2))  r_ptr->flags9 |= RF9_S_HI_DEMON;
	if(lev > 19) r_ptr->flags9 |= RF9_TELE_APPROACH;
	if(lev > 49 && (lev % 7 == 3))  r_ptr->flags9 |= RF9_TELE_HI_APPROACH;

	if(lev > 9) r_ptr->flags9 |= RF9_BEAM_LITE;
	if(lev > 89 && (lev%7 == 4)) r_ptr->flags9 |= RF9_HELLFIRE;
	if(lev > 64 && (lev % 7 == 1))  r_ptr->flags9 |= RF9_MAELSTROM;

	r_ptr->flagsr = 0L;
	if(lev > 59 || (lev % 5 == 0))  r_ptr->flagsr |= RFR_IM_ACID;
	if(lev > 29 || (lev % 5 == 1))  r_ptr->flagsr |= RFR_IM_ELEC;
	if(lev > 9 || (lev % 5 == 2))  r_ptr->flagsr |= RFR_IM_FIRE;
	if(lev > 24 || (lev % 5 == 3))  r_ptr->flagsr |= RFR_IM_COLD;
	if(lev >  9 || (lev % 5 == 4))  r_ptr->flagsr |= RFR_IM_POIS;

	if(lev > 19) r_ptr->flagsr |= RFR_RES_LITE;
	if(lev > 19 && (lev % 7 == 2)) r_ptr->flagsr |= RFR_RES_DARK;
	if(lev > 19 && (lev % 7 == 0)) r_ptr->flagsr |= RFR_RES_NETH;
	if(lev % 7 == 1) r_ptr->flagsr |= RFR_RES_WATE;
	if(lev > 49 || (lev % 7 == 3)) r_ptr->flagsr |= RFR_RES_CHAO;
	if((lev % 7 == 3) || (lev % 7 == 5)) r_ptr->flagsr |= RFR_RES_TELE;


	//v1.1.84 新クエスト「連続昏睡事件Ⅰ」用
	//クエスト受領中にレベル41~49の魔理沙が生成されたらフラグを書き換える

	if (quest[QUEST_HANGOKU1].status == QUEST_STATUS_TAKEN && !a_info[ART_HANGOKU_SAKE].cur_num &&
		lev > (quest[QUEST_HANGOKU1].level-5) && lev < (quest[QUEST_HANGOKU1].level + 5))
	{
		if (cheat_hear) msg_format(_("魔理沙がTROPHYフラグ付きで生成された", "Marisa generated with TROPHY flag"));
		r_ptr->flags2 |= RF2_TROPHY;
		r_ptr->flags3 |= RF3_UNDEAD;

		r_ptr->blow[1].effect = RBE_CURSE;
		r_ptr->blow[3].effect = RBE_DR_MANA;

		r_ptr->flagsr |= RFR_RES_NETH;
		r_ptr->flags5 |= RF5_CAUSE_4;
		r_ptr->flags5 |= RF5_BA_NETH;
		r_ptr->flags5 |= RF5_BRAIN_SMASH;
		r_ptr->flags6 |= RF6_HAND_DOOM;

	}



	return TRUE;
}


/*:::アイテムを指定し、それに合わせて付喪神のパラメータを設定する。*/
/*:::呪文詠唱時にこの関数が呼ばれたときはこのアイテムを元にしたモンスターがまだ生成されていないことに注意。*/
bool apply_mon_race_tsukumoweapon(object_type *o_ptr, int r_idx)
{
	int lev;
	int dd,ds;
	int pval = 0;
	int rbm;
	bool blade;

	u32b flgs[TR_FLAG_SIZE];

	monster_race	*r_ptr = &r_info[r_idx];
	object_kind *k_ptr = &k_info[o_ptr->k_idx];
	if(!(r_ptr->flags7 & RF7_VARIABLE))
	{
		msg_format(_("ERROR:livingweaponパラメータ設定でVARIABLEフラグのないr_ptrが渡された",
                    "ERROR: r_ptr entry without VARIABLE flag passed as livingweapon parameter"));
		return FALSE;
	}
	if(!o_ptr->k_idx)
	{
		msg_format(_("ERROR:生きた武器(idx:%d)のベースアイテムがない",
                    "ERROR: Living weapon (idx:%d) has no base item"),r_idx);
		return FALSE;
	}
	blade = object_has_a_blade(o_ptr);
	if(!object_is_melee_weapon(o_ptr))
	{
		msg_format(_("ERROR:apply_param_livingweapon()にて近接武器以外が呼ばれた",
                    "ERROR: apply_param_livingweapon() called for a non-melee weapon"));
		return FALSE;
	}

	if(o_ptr->tval == TV_SWORD || o_ptr->tval == TV_KATANA || o_ptr->tval == TV_AXE) rbm = RBM_SLASH;
	else if(o_ptr->tval == TV_KNIFE ) rbm = RBM_STING;
	else if(o_ptr->tval == TV_SPEAR) rbm = RBM_SPEAR;
	else if(o_ptr->tval == TV_HAMMER) rbm = RBM_PUNCH;
	else if(o_ptr->tval == TV_STICK) rbm = RBM_STRIKE;
	else if(object_has_a_blade(o_ptr)) rbm = RBM_SLASH;
	else rbm = RBM_HIT;

	//画面表示をアイテムと同じにする
	r_ptr->d_attr = k_ptr->d_attr;
	r_ptr->d_char = k_ptr->d_char;
	r_ptr->x_attr = k_ptr->x_attr;
	r_ptr->x_char = k_ptr->x_char;

	lev = k_info[o_ptr->k_idx].level;
	if(object_is_fixed_artifact(o_ptr) && lev < a_info[o_ptr->name1].level) lev = a_info[o_ptr->name1].level;
	else if(object_is_random_artifact(o_ptr) && lev < object_value_real(o_ptr) / 1000) lev = object_value_real(o_ptr) / 1000;
	else if(object_is_ego(o_ptr) && lev < e_info[o_ptr->name2].level) lev = e_info[o_ptr->name2].level;

	if(object_value_real(o_ptr) / 2000 > lev)lev = object_value_real(o_ptr) / 2000;
	if(lev > 99) lev = 99;
	if(lev < 1) lev = 1;
	object_flags(o_ptr,flgs);
	pval = o_ptr->pval;

	r_ptr->level = lev;

	r_ptr->mexp = lev * 10;

	r_ptr->hdice = MIN(200,(lev/2 + o_ptr->weight/20));
	if(object_is_artifact(o_ptr)) r_ptr->hside = lev;
	else if(object_is_ego(o_ptr)) r_ptr->hside = lev * 2 / 3;
	else r_ptr->hside = 1 + lev / 2;

	if(r_ptr->hdice < 10) r_ptr->hdice = 10;
	if(r_ptr->hside < 1) r_ptr->hside = 1;

	r_ptr->ac = lev + o_ptr->to_a * 5;
	r_ptr->aaf = 20;
	r_ptr->speed = 110 + lev / 5;
	//v1.1.27削除
	//r_ptr->ac += o_ptr->to_a;

	dd = o_ptr->dd ;
	ds = o_ptr->ds ;
	dd += o_ptr->to_d / 4;
	ds += o_ptr->to_h / 4;



	/*:::flags初期化*/
	r_ptr->flags1 = (RF1_FORCE_MAXHP);
	r_ptr->flags2 = (RF2_COLD_BLOOD | RF2_EMPTY_MIND | RF2_EPHEMERA);
	r_ptr->flags3 = (RF3_KWAI | RF3_NONLIVING | RF3_NO_CONF |  RF3_NO_FEAR | RF3_NO_STUN);
	r_ptr->flags4 = (0L);
	r_ptr->flags5 = (0L);
	r_ptr->flags6 = (0L);
	r_ptr->flags7 = (RF7_CAN_FLY | RF7_VARIABLE);
	r_ptr->flags8 = (0L);
	r_ptr->flags9 = (0L);
	r_ptr->flagsr = (0L);
	r_ptr->blow[0].method = 0;
	r_ptr->blow[1].method = 0;
	r_ptr->blow[2].method = 0;
	r_ptr->blow[3].method = 0;

	if(object_is_cursed(o_ptr)) r_ptr->flags1 |= RF1_RAND_50;

	if(have_flag(flgs, TR_STR)) dd += pval;
	if(have_flag(flgs, TR_INT)) r_ptr->ac += pval * 5;
	if(have_flag(flgs, TR_WIS)) r_ptr->hside += pval * 5;
	if(have_flag(flgs, TR_CON)) r_ptr->hside += pval * 5;
	if(have_flag(flgs, TR_DEX)) ds += pval;
	if(have_flag(flgs, TR_CHR)) r_ptr->ac += pval * 5;

	if(have_flag(flgs, TR_MAGIC_MASTERY)) ;
	if(have_flag(flgs, TR_STEALTH)) r_ptr->ac += pval * 5;
	if(have_flag(flgs, TR_SEARCH)) r_ptr->aaf = MIN(100, r_ptr->aaf + pval * 10);
	if(have_flag(flgs, TR_INFRA));
	if(have_flag(flgs, TR_TUNNEL)) r_ptr->flags2 |= RF2_KILL_WALL;

	if(have_flag(flgs, TR_BLOWS)) r_ptr->speed += pval * 5;
	if(have_flag(flgs, TR_SPEED)) r_ptr->speed += pval * 3;

	if(have_flag(flgs, TR_KILL_EVIL)) dd += 3;
	if(have_flag(flgs, TR_KILL_GOOD)) dd += 3;
	if(have_flag(flgs, TR_KILL_ANIMAL)) dd += 2;
	if(have_flag(flgs, TR_KILL_HUMAN)) dd += 2;
	if(have_flag(flgs, TR_KILL_UNDEAD)) dd += 2;
	if(have_flag(flgs, TR_KILL_DRAGON)) dd += 2;
	if(have_flag(flgs, TR_KILL_DEITY)) dd += 2;
	if(have_flag(flgs, TR_KILL_DEMON)) dd += 2;
	if(have_flag(flgs, TR_KILL_KWAI)) dd += 2;

	if(have_flag(flgs, TR_IM_ACID)) ds += 3;
	if(have_flag(flgs, TR_IM_ELEC)) ds += 3;
	if(have_flag(flgs, TR_IM_FIRE)) ds += 3;
	if(have_flag(flgs, TR_IM_COLD)) ds += 3;

	if(have_flag(flgs, TR_SH_FIRE))  r_ptr->flags2 |= RF2_AURA_FIRE;
	if(have_flag(flgs, TR_SH_COLD))  r_ptr->flags2 |= RF2_AURA_COLD;
	if(have_flag(flgs, TR_SH_ELEC))  r_ptr->flags2 |= RF2_AURA_ELEC;


	if(have_flag(flgs, TR_EX_VORPAL)) dd = dd * 5 / 3;
	else if(have_flag(flgs, TR_VORPAL)) dd = dd * 6 / 5;

	if(dd < 1) dd = 1;
	if(ds < 1) ds = 1;


	if(have_flag(flgs, TR_REFLECT)) r_ptr->flags2 |= RF2_REFLECTING;

	r_ptr->blow[0].method = rbm;
	if(have_flag(flgs, TR_EX_VORPAL) ) r_ptr->blow[0].effect = RBE_SUPERHURT;
	else if(have_flag(flgs, TR_IMPACT) ) r_ptr->blow[0].effect = RBE_SHATTER;
	else if(have_flag(flgs, TR_SLAY_GOOD)) r_ptr->blow[0].effect = RBE_KILL;
	else if(have_flag(flgs, TR_SLAY_DRAGON)) r_ptr->blow[0].effect = RBE_STUN;
	else r_ptr->blow[0].effect = RBE_HURT;
	r_ptr->blow[0].d_dice = dd;
	r_ptr->blow[0].d_side = ds;


	if(lev > 9)
	{
		r_ptr->blow[1].method = rbm;
		if(have_flag(flgs, TR_FORCE_WEAPON)) r_ptr->blow[1].effect = RBE_SUPERHURT;
		else if(have_flag(flgs, TR_SLAY_EVIL)) r_ptr->blow[1].effect = RBE_SMITE;
		else if(have_flag(flgs, TR_SLAY_UNDEAD)) r_ptr->blow[1].effect = RBE_TERRIFY;
		else if(have_flag(flgs, TR_SLAY_KWAI)) r_ptr->blow[1].effect = RBE_CONFUSE;
		else r_ptr->blow[1].effect = RBE_HURT;
		r_ptr->blow[1].d_dice = dd;
		r_ptr->blow[1].d_side = ds;
	}
	if(lev > 19)
	{
		r_ptr->blow[2].method = rbm;
		if(have_flag(flgs, TR_VAMPIRIC)) r_ptr->blow[2].effect = RBE_EXP_VAMP;
		else if(have_flag(flgs, TR_BRAND_POIS)) r_ptr->blow[2].effect = RBE_POISON;
		else if(have_flag(flgs, TR_BRAND_ELEC)) r_ptr->blow[2].effect = RBE_ELEC;
		else if(have_flag(flgs, TR_BRAND_FIRE)) r_ptr->blow[2].effect = RBE_FIRE;
		else if(have_flag(flgs, TR_SLAY_HUMAN)) r_ptr->blow[2].effect = RBE_BLEED;
		else if(have_flag(flgs, TR_SLAY_DEITY)) r_ptr->blow[2].effect = RBE_TIME;
		else r_ptr->blow[2].effect = RBE_HURT;
		r_ptr->blow[2].d_dice = dd;
		r_ptr->blow[2].d_side = ds;
		if(r_ptr->blow[2].effect == RBE_ELEC || r_ptr->blow[2].effect == RBE_FIRE) 	r_ptr->blow[2].d_side *= 2;
	}
	if(lev > 29)
	{
		r_ptr->blow[3].method = rbm;
		if(have_flag(flgs, TR_CHAOTIC)) r_ptr->blow[3].effect = RBE_CHAOS;
		else if(have_flag(flgs, TR_BRAND_ACID)) r_ptr->blow[3].effect = RBE_ACID;
		else if(have_flag(flgs, TR_BRAND_COLD)) r_ptr->blow[3].effect = RBE_COLD;
		else if(have_flag(flgs, TR_SLAY_ANIMAL)) r_ptr->blow[3].effect = RBE_PARALYZE;
		else if(have_flag(flgs, TR_SLAY_DEMON)) r_ptr->blow[3].effect = RBE_UN_BONUS;
		else r_ptr->blow[3].effect = RBE_HURT;
		r_ptr->blow[3].d_dice = dd;
		r_ptr->blow[3].d_side = ds;
		if(r_ptr->blow[3].effect == RBE_ACID || r_ptr->blow[3].effect == RBE_COLD) 	r_ptr->blow[3].d_side *= 2;
	}

	if(have_flag(flgs, TR_IM_ACID) || have_flag(flgs, TR_RES_ACID) ) r_ptr->flagsr |= RFR_IM_ACID;
	if(have_flag(flgs, TR_IM_ELEC) || have_flag(flgs, TR_RES_ELEC) ) r_ptr->flagsr |= RFR_IM_ELEC;
	if(have_flag(flgs, TR_IM_FIRE) || have_flag(flgs, TR_RES_FIRE) ) r_ptr->flagsr |= RFR_IM_FIRE;
	if(have_flag(flgs, TR_IM_COLD) || have_flag(flgs, TR_RES_COLD) ) r_ptr->flagsr |= RFR_IM_COLD;

	if( have_flag(flgs, TR_RES_POIS)) r_ptr->flagsr |= RFR_IM_POIS;
	if( have_flag(flgs, TR_RES_LITE)) r_ptr->flagsr |= RFR_RES_LITE;
	if( have_flag(flgs, TR_RES_DARK)) r_ptr->flagsr |= RFR_RES_DARK;
	if( have_flag(flgs, TR_RES_SHARDS)) r_ptr->flagsr |= RFR_RES_SHAR;
	if( have_flag(flgs, TR_RES_SOUND)) r_ptr->flagsr |= RFR_RES_SOUN;
	if( have_flag(flgs, TR_RES_NETHER)) r_ptr->flagsr |= RFR_RES_NETH;
	if( have_flag(flgs, TR_RES_WATER)) r_ptr->flagsr |= RFR_RES_WATE;
	if( have_flag(flgs, TR_RES_CHAOS)) r_ptr->flagsr |= RFR_RES_CHAO;
	if( have_flag(flgs, TR_RES_DISEN)) r_ptr->flagsr |= RFR_RES_DISE;
	if( have_flag(flgs, TR_RES_HOLY)) r_ptr->flagsr |= RFR_RES_HOLY;
	if( have_flag(flgs, TR_RES_TIME)) r_ptr->flagsr |= RFR_RES_TIME;
	if( have_flag(flgs, TR_NO_TELE)) r_ptr->flagsr |= RFR_RES_TELE;


	if(r_ptr->ac < 0) r_ptr->ac = 0;

	return TRUE;

}





/*:::可変パラメータモンスターのパラメータ設定*/
bool apply_variable_monster(int r_idx)
{
	if(r_idx == MON_REIMU)
		return apply_mon_race_reimu();
	else if(r_idx == MON_MARISA)
		return apply_mon_race_marisa();
	else if(r_idx == MON_MASTER_KAGUYA)
		return apply_mon_race_master_kaguya();
	///mod140719 ランダムユニーク実装
	else if(IS_RANDOM_UNIQUE_IDX(r_idx))
		return apply_random_unique(r_idx);
	else if(r_idx >= MON_TSUKUMO_WEAPON1 && r_idx <= MON_TSUKUMO_WEAPON5 ) return TRUE; //アイテムポインタが要るのでほかで処理

	else if(r_idx == MON_BANKI_HEAD_1 || r_idx == MON_BANKI_HEAD_2)
		return apply_mon_race_banki_head(r_idx);

	else if(r_idx == MON_GREEN_EYED)
		return apply_mon_race_green_eyed();


	else msg_format(_("ERROR:idx(%d)の可変パラメータモンスターのパラメータ処理が登録されていない",
                        "ERROR: Parameter logic not defined for variable parameter monster idx (%d)"),r_idx);

	return FALSE;
}

//v1.1.57 隠岐奈「秘神の暗躍弾幕」でダメージを食らうモンスターの判定とダメージ処理
//この処理はモンスター生成終了直前に呼ばれる
void process_pattern_attack(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	int attack_type = p_ptr->magic_num2[0];
	int plev = p_ptr->lev;

	int dam, dam_ratio;

	if (p_ptr->pclass != CLASS_OKINA) return;
	if (m_ptr->r_idx < 1) return;
	if (!attack_type) return;

	if (!is_hostile(m_ptr)) return;

	//クエスト中にも効くようにしてしまう
	if (p_ptr->inside_battle || p_ptr->inside_arena) return;

	if (attack_type < 0 || attack_type > PATTERN_ATTACK_TYPE_MAX) { msg_format(_("ERROR:magic_num2[0]の値がおかしい(%d)",
                                                                                "ERROR: magic_num2[0] value is weird (%d)"), attack_type); return; }
	if (r_ptr->flagsr & RFR_RES_ALL) return;

	//選択した攻撃タイプに該当しないモンスターには何もせず終了
	if (!(r_ptr->flags3 & pattern_attack_table[attack_type].rf3_flag)) return;


	//ダメージ与えて起こす

	if (r_ptr->flags1 & RF1_QUESTOR) dam_ratio = plev / 2;
	else if (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) dam_ratio = plev;
	else dam_ratio = plev * 2;

	dam = randint1(m_ptr->hp * dam_ratio / 100);

	if (dam < plev * 10) dam = plev * 10;
	if (dam > plev * 100) dam = plev * 100;

	m_ptr->hp -= dam;
	if (m_ptr->hp < 0) m_ptr->hp = 0;
	set_monster_csleep(m_idx, 0);

	set_hostile(m_ptr);

}

/*
 * Attempt to place a monster of the given race at the given location.
 *
 * To give the player a sporting chance, any monster that appears in
 * line-of-sight and is extremely dangerous can be marked as
 * "FORCE_SLEEP", which will cause them to be placed with low energy,
 * which often (but not always) lets the player move before they do.
 *
 * This routine refuses to place out-of-depth "FORCE_DEPTH" monsters.
 *
 * XXX XXX XXX Use special "here" and "dead" flags for unique monsters,
 * remove old "cur_num" and "max_num" fields.
 *
 * XXX XXX XXX Actually, do something similar for artifacts, to simplify
 * the "preserve" mode, and to make the "what artifacts" flag more useful.
 *
 * This is the only function which may place a monster in the dungeon,
 * except for the savefile loading code.
 */
/*:::指定したインデックスのモンスターを指定した座標に配置する。*/
/*:::m_list[]とcave[][]にモンスターの個体情報を登録する。*/
/*:::警告や特殊モンスター処理も*/
static bool place_monster_one(int who, int y, int x, int r_idx, u32b mode)
{
	/* Access the location */
	cave_type		*c_ptr = &cave[y][x];

	monster_type	*m_ptr;

	monster_race	*r_ptr = &r_info[r_idx];

	cptr		name = (r_name + r_ptr->name);

	int cmi;

	/* DO NOT PLACE A MONSTER IN THE SMALL SCALE WILDERNESS !!! */
	if (p_ptr->wild_mode) return FALSE;

	/* Verify location */
	if (!in_bounds(y, x)) return (FALSE);

	/* Paranoia */
	if (!r_idx) return (FALSE);

	/* Paranoia */
	if (!r_ptr->name) return (FALSE);

	if (cheat_hear) msg_format("mode:%04x", mode);


	/*:::パターン上や進入不可能地形には配置できない。ただしチェンジモンスターの再生成のときはPM_IGNORE_TERRAINがONになってて配置可能である*/
	if (!(mode & PM_IGNORE_TERRAIN))
	{
		/* Not on the Pattern */
		if (pattern_tile(y, x)) return FALSE;

		/* Require empty space (if not ghostly) */
		if (!monster_can_enter(y, x, r_ptr, 0)) return FALSE;
	}

	///mod150907 夢の世界では一部の敵が出ない
	if(d_info[dungeon_type].flags1 & DF1_DREAM_WORLD)
	{
		if(r_ptr->flags2 & RF2_EMPTY_MIND && !(mode & (PM_EPHEMERA | PM_FORCE_PET)))
		{
			return FALSE;
		}
	}

	//v.1.1.78 敵対する相手が友好召喚されるとき召喚抑止
	//process_monster()の反感処理と揃えたいのだが、場合によってはクエストの生成時に影響するかもしれないからここにそのまま反映はしづらい。
	if (mode & PM_FORCE_FRIENDLY)
	{
		if ((p_ptr->prace && RACE_SENNIN || p_ptr->prace && RACE_TENNIN) && r_ptr->flags7 & RF7_FROM_HELL) return FALSE;




	}


/*:::特殊なモンスターの生成禁止処理など*/
	///mon
	if (!p_ptr->inside_battle && !p_ptr->inside_arena && !(mode & PM_EPHEMERA))
	{
		/*:::ユニークと準ユニークは生成可能数を超えて生成されない*/
		/* Hack -- "unique" monsters must be "unique" */
		if (((r_ptr->flags1 & (RF1_UNIQUE)) ||
		     (r_ptr->flags7 & (RF7_NAZGUL))) &&
		    (r_ptr->cur_num >= r_ptr->max_num))
		{
			/* Cannot create */
			return (FALSE);
		}

		if ((r_ptr->flags7 & (RF7_UNIQUE2)) &&
		    (r_ptr->cur_num >= 1))
		{
			return (FALSE);
		}

		if (r_idx == MON_BANORLUPART)
		{
			if (r_info[MON_BANOR].cur_num > 0) return FALSE;
			if (r_info[MON_LUPART].cur_num > 0) return FALSE;
		}

		if(IS_RANDOM_UNIQUE_IDX(r_idx))
		{
			if( dun_level <10)
			{
				if(cheat_hear) msg_format(_("10階未満のためランダムユニークが発生抑止された", "Random unique suppressed at dungeon level less than 10."));
				return FALSE;
			}
		}


		/*:::Hack - モズグス様生存時は変身後のモズグス様が生成されない*/
		if (r_idx == MON_MOZGUS2)
		{
			if(r_info[MON_MOZGUS].r_akills == 0) return FALSE;
		}

		//v1.1.42 HACK:紫苑本気モードは通常紫苑が倒された後でしか出ない
		if (r_idx == MON_SHION_2)
		{
			if (r_info[MON_SHION_1].r_akills == 0) return FALSE;
		}

		//ヘカーティアの体は二つ以上出現しない(闘技場除く)
		if(r_idx >= MON_HECATIA1 && r_idx <= MON_HECATIA3 && !p_ptr->inside_battle)
		{
			int i;
			for (i = 1; i < m_max; i++)
			{
				if(m_list[i].r_idx >= MON_HECATIA1 && m_list[i].r_idx <= MON_HECATIA3)
					return FALSE;
			}
		}

		/*:::賞金首フラグをもつ敵は、該当クエストが受領済みでないと出現しない*/
		if(r_ptr->flags3 & RF3_WANTED)
		{
			int cnt;
			bool ok = FALSE;
			for (cnt = max_quests - 1; cnt > 0; cnt--)
			{
				if (quest[cnt].status != QUEST_STATUS_TAKEN || quest[cnt].type != QUEST_TYPE_BOUNTY_HUNT)continue;
				if(quest[cnt].r_idx != r_idx) continue;

				ok = TRUE;
				break;
			}

			if(!ok)
			{
				if(cheat_hear) msg_format(_("お尋ね者(ID:%d)がクエスト未受領のため発生抑止された",
                                            "Generation stopped for wanted person (ID: %d) because quest was not accepted."),r_idx);
				return FALSE;
			}

			if(quest_number(dun_level))
			{
				if(cheat_hear) msg_print(_("現在階層がクエスト中のためお尋ね者の生成が抑止された。",
                                            "Generation stopped for wanted person because current dungeon level is quest."));
				return FALSE;
			}

		}

		/*:::ランダムユニーク、可変パラメータユニークは地上では出ない*/
		//輝夜除く
		if ((r_ptr->flags7 & RF7_UNIQUE2) && (r_ptr->flags7 & RF7_VARIABLE) && (!dun_level) && r_idx != MON_MASTER_KAGUYA)
		{
				if(cheat_hear) msg_print(_("地上のため特殊ユニークの生成が抑止された。",
                                            "Generation stopped for special unique monster because player is on surface."));
				return FALSE;

		}

		//v1.1.29 霊夢と魔理沙(パラメータ階層依存特殊ユニーク)は配下にして階層をまたいで連れ回すと何かパラメータおかしくなるので生成抑止
		//(現在フロア限定配下(PM_EPHEMERA)なら出る)
		if((r_idx == MON_REIMU || r_idx == MON_MARISA) && (mode & PM_FORCE_PET))
		{
			if(cheat_hear) msg_format(_("レイマリは通常配下としては出現しない。",
                                        "Rei/Mari cannot normally appear as followers."));
			return FALSE;
		}


		///mod150528 モンスターボールなどのときは抑止されないように修正
		if(!check_rest_f50(r_idx) && !(mode & PM_FORCE_PET))
		{
			if(quest_number(dun_level) && !character_dungeon && dungeon_type != DUNGEON_ANGBAND)
			{
				if(cheat_hear) msg_format(_("低難度設定だがクエスト生成中のため強敵の生成が抑止されない。(IDX:%d)",
                                            "Strong enemy was generated in quest even though difficulty setting is low. (IDX: %d)"),r_idx);

			}
			else
			{
				if(cheat_hear) msg_format(_("難易度設定により強敵の生成が抑止された。(IDX:%d)",
                                            "Generation stopped for strong enemy because difficulty setting is low. (IDX: %d)"),r_idx);
				return FALSE;
			}
		}



		/*:::ランダムユニーク、可変パラメータユニークのクエストダンジョン発生抑止　ランクエ階、紫、オベロンの階では出る あとさとりクエストでは魔理沙が出る*/
		//永琳のときの輝夜は出る
		//v1.1.42 「夢は現実化する」クエストのときはランダムユニーク1を出す
		if ((r_ptr->flags7 & RF7_UNIQUE2) && (r_ptr->flags7 & RF7_VARIABLE)
			&& !(mode & PM_ALLOW_SP_UNIQUE)
			&& (dungeon_type != DUNGEON_ANGBAND)
			&& (quest_number(dun_level) && !(p_ptr->inside_quest == QUEST_SATORI || p_ptr->inside_quest == QUEST_REIMU_ATTACK && r_idx == MON_REIMU || p_ptr->inside_quest == QUEST_DREAMDWELLER && r_idx == MON_RANDOM_UNIQUE_1))
			&& r_idx != MON_MASTER_KAGUYA)
		{
				if(cheat_hear) msg_print(_("クエストダンジョンのため特殊ユニークの生成が抑止された。",
                                            "Generation stopped for special unique monsters because this is a quest level."));
				return FALSE;

		}


		///mod141115 モズグス様はクエストダンジョンに出ないことにする
		if (r_idx == MON_MOZGUS)
		{
			if( p_ptr->inside_quest) return FALSE;
		}

		///v1.1.42 紫苑もクエストダンジョンに出ないことにする
		if (r_idx == MON_SHION_1 || r_idx == MON_SHION_2)
		{
			if (p_ptr->inside_quest) return FALSE;
		}


		/* Depth monsters may NOT be created out of depth, unless in Nightmare mode */
		if ((r_ptr->flags1 & (RF1_FORCE_DEPTH)) && (dun_level < r_ptr->level) &&
		    (!ironman_nightmare || (r_ptr->flags1 & (RF1_QUESTOR))))
		{
			/* Cannot create */
			return (FALSE);
		}
	}

/*:::ランクエや特定数打倒クエストでは対象敵は規定数以上生成されない*/
	if (quest_number(dun_level) && !(mode & PM_EPHEMERA) )
	{
		int hoge = quest_number(dun_level);
		if ((quest[hoge].type == QUEST_TYPE_KILL_LEVEL) || (quest[hoge].type == QUEST_TYPE_RANDOM))
		{
			if(r_idx == quest[hoge].r_idx)
			{
				int number_mon, i2, j2;
				number_mon = 0;

				/* Count all quest monsters */
				for (i2 = 0; i2 < cur_wid; ++i2)
					for (j2 = 0; j2 < cur_hgt; j2++)
						if (cave[j2][i2].m_idx > 0)
							if (m_list[cave[j2][i2].m_idx].r_idx == quest[hoge].r_idx)
								number_mon++;
				if(number_mon + quest[hoge].cur_num >= quest[hoge].max_num)
					return FALSE;
			}
		}
	}

/*:::守りのルーンの召喚防止処理、もしくはルーン破壊処理*/
	if (is_glyph_grid(c_ptr))
	{
		if (randint1(BREAK_GLYPH) < (r_ptr->level+20))
		{
			/* Describe observable breakage */
			if (c_ptr->info & CAVE_MARK)
			{
#ifdef JP
msg_print("守りのルーンが壊れた！");
#else
				msg_print("The rune of protection is broken!");
#endif

			}

			/* Forget the rune */
			c_ptr->info &= ~(CAVE_MARK);

			/* Break the rune */
			c_ptr->info &= ~(CAVE_OBJECT);
			c_ptr->mimic = 0;

			/* Notice */
			note_spot(y, x);
		}
		else return FALSE;
	}

	/*::: -GIGA HACK- 可変パラメータモンスターは生成時にr_info[]を書き換える*/
	/*:::ルーン破壊判定は前回のレベルになるがまあ問題はないだろう。*/
	///mod150604 映姫の浄頗梨審判のときはr_idxを再計算しないよう処理
	///mod150614 新闘技場ではモンスター選定時に既にパラメータ計算をしているので再計算しない
	if((r_ptr->flags7 & RF7_VARIABLE) && !(p_ptr->pclass == CLASS_EIKI && mode & PM_EPHEMERA) && !p_ptr->inside_battle)
	{
		if(!apply_variable_monster(r_idx))
		{
			msg_format(_("ERROR:idx(%d)の可変パラメータ適用に失敗した",
                        "ERROR: Failed to apply variable monster idx (%d)"),r_idx);
			return FALSE;
		}
	}
	if((r_ptr->flags3 & RF3_WANTED) && (cheat_hear)) msg_format(_("お尋ね者 (%s)が生成された。",
                                                                    "Wanted person (%s) generated."), name);

/*:::ユニークや深いレベルのモンスターが出たら知らせる（チートオプション）*/
	/* Powerful monster */
	if (r_ptr->level > dun_level)
	{
		if(cheat_hear && (r_ptr->flags1 & (RF1_FORCE_DEPTH)) && r_ptr->level > dun_level)
			msg_format("FORCE_DEPTH(%s).", name);

		/* Unique monsters */
		if (r_ptr->flags1 & (RF1_UNIQUE))
		{
			/* Message for cheaters */
#ifdef JP
			if (cheat_hear) msg_format("深層のユニーク・モンスター (%s)。", name);
#else
			if (cheat_hear) msg_format("Deep Unique (%s).", name);
#endif
		}

		/* Normal monsters */
		else
		{
			/* Message for cheaters */
#ifdef JP
			if (cheat_hear) msg_format("深層のモンスター (%s)。", name);
#else
			if (cheat_hear) msg_format("Deep Monster (%s).", name);
#endif
		}
	}

	/* Note the monster */
	else if (r_ptr->flags1 & (RF1_UNIQUE))
	{
		/* Unique monsters induce message */
#ifdef JP
		if (cheat_hear) msg_format("ユニーク・モンスター (%s)。", name);
#else
		if (cheat_hear) msg_format("Unique (%s).", name);
#endif

	}

	else if ((r_ptr->flags7 & RF7_UNIQUE2) && (r_ptr->flags7 & RF7_VARIABLE))
	{

		if (cheat_hear) msg_format(_("特殊ユニークモンスターが生成された (idx:%d)",
                                        "Special unique monster generated (idx: %d)"),r_idx);
	}







	/*:::ユニークとナズグルは怪しい影にならない・・上のほうのルーチンでも似た事やった気がするが。*/
	if ((r_ptr->flags7 & RF7_UNIQUE2) ||(r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_NAZGUL) || (r_ptr->level < 10)) mode &= ~PM_KAGE;

	///mod140417 特殊処理　ぬえは必ず怪しい影状態で生成
	if(r_idx == MON_NUE && !p_ptr->inside_battle) mode |= PM_KAGE;

	//v1.1.68 クエスト打倒対象モンスターは強制敵対
	if (p_ptr->inside_quest && quest[p_ptr->inside_quest].r_idx == r_idx)
		mode |= PM_FORCE_ENEMY;


/*:::ここからモンスターの個体情報をダンジョンに登録する処理*/
	/* Make a new monster */
	/*:::m_pop():m_list[]の中の使われてない部分を見つけて添え字を返す*/
	c_ptr->m_idx = m_pop();

	/*:::「最後に生成されたモンスターのm_list[]のインデックス」としてあとで使われる？*/
	hack_m_idx_ii = c_ptr->m_idx;

	/* Mega-Hack -- catch "failure" */
	/*:::モンスターが多すぎてm_listに空きがなかった場合？*/
	if (!c_ptr->m_idx) return (FALSE);


	/* Get a new monster record */
	/*:::m_listの該当箇所に実際にモンスターのパラメータ情報を登録する*/
	m_ptr = &m_list[c_ptr->m_idx];

	//v1.1.81 モンスター生成時にランダムな数値を付与する
	m_ptr->mon_random_number = randint0(65536);

	/* Save the race */
	m_ptr->r_idx = r_idx;
	/*:::たぬき以外は初期ap_r_idxとr_idxが同じのはず*/
	m_ptr->ap_r_idx = initial_r_appearance(r_idx);

	/* No flags */
	m_ptr->mflag = 0;
	m_ptr->mflag2 = 0;

	/* Hack -- Appearance transfer */
	/*:::クローンモンスターか増殖のとき増殖もとのモンスターの表示タイプと実際のタイプが異なってたらそれに合わせる*/
	if ((mode & PM_MULTIPLY) && (who > 0) && !is_original_ap(&m_list[who]))
	{
		m_ptr->ap_r_idx = m_list[who].ap_r_idx;

		/* Hack -- Shadower spawns Shadower */
		if (m_list[who].mflag2 & MFLAG2_KAGE) m_ptr->mflag2 |= MFLAG2_KAGE;
	}

	//v1.1.93 特殊フラグを立てて召喚するモード
	if (mode & PM_SET_MFLAG_SP)
	{
		m_ptr->mflag |= MFLAG_SPECIAL;
	}



	/*::: -Mega Hack- 狐狸戦争のときは選択した陣営に合わせて友好度設定 仮に狐=秩序、狸=混沌陣営にしておく。*/
	if(p_ptr->inside_quest == QUEST_KORI)
	{
		if(r_idx == MON_NUE || r_idx == MON_MAMIZOU || r_idx == MON_TANUKI || r_idx == MON_O_TANUKI)
		{
			m_ptr->sub_align |= SUB_ALIGN_EVIL;
			if(quest[QUEST_KORI].flags & QUEST_FLAG_TEAM_B) set_friendly(m_ptr);
		}
		else if( r_idx == MON_RAN || r_idx == MON_CHAMELEON)
		{
			m_ptr->sub_align |= SUB_ALIGN_GOOD;
			if(quest[QUEST_KORI].flags & QUEST_FLAG_TEAM_A) set_friendly(m_ptr);
		}
		else if(who > 0)
			m_ptr->sub_align = m_list[who].sub_align;
		else
			m_ptr->sub_align = SUB_ALIGN_NEUTRAL;

	}
	//v1.1.91 ヤクザ戦争も
	else if (p_ptr->inside_quest == QUEST_YAKUZA_1)
	{

		switch (r_idx)
		{
		case MON_SAKI:
		case MON_ANIMAL_G_KEIGA:
			m_ptr->sub_align = SUB_ALIGN_TEAM_A;
			if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KEIGA) set_friendly(m_ptr);
			break;
		case MON_YACHIE:
		case MON_ANIMAL_G_KIKETSU:
		case MON_WYRM_RED:
			m_ptr->sub_align = SUB_ALIGN_TEAM_B;
			if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KIKETSU) set_friendly(m_ptr);
			break;
		case MON_YUMA:
		case MON_ANIMAL_G_GOUYOKU:
			m_ptr->sub_align = SUB_ALIGN_TEAM_C;
			if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_GOUYOKU) set_friendly(m_ptr);
			break;
		case MON_MAYUMI:
		case MON_HANIWA_A1:
		case MON_HANIWA_A2:
		case MON_HANIWA_C1:
		case MON_HANIWA_C2:
		case MON_HANIWA_F1:
		case MON_HANIWA_F2:
			m_ptr->sub_align = SUB_ALIGN_TEAM_D;
			if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_HANIWA) set_friendly(m_ptr);
			break;

		default:
			if (who > 0)
				m_ptr->sub_align = m_list[who].sub_align;
			else
				m_ptr->sub_align = SUB_ALIGN_NEUTRAL;

		}

	}

	/* Sub-alignment of a monster */
	///mon 勢力関係
	/*:::善悪どちらの勢力に呼ばれたかでsub_align値が変化*/
	else if ((who > 0) && !(r_ptr->flags3 & (RF3_ANG_CHAOS | RF3_ANG_COSMOS)))
		m_ptr->sub_align = m_list[who].sub_align;
	else
	{
		m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
		if (r_ptr->flags3 & RF3_ANG_CHAOS) m_ptr->sub_align |= SUB_ALIGN_EVIL;
		if (r_ptr->flags3 & RF3_ANG_COSMOS) m_ptr->sub_align |= SUB_ALIGN_GOOD;
	}

	/* Place the monster at the location */
	m_ptr->fy = y;
	m_ptr->fx = x;


	/* No "timed status" yet */
	for (cmi = 0; cmi < MAX_MTIMED; cmi++) m_ptr->mtimed[cmi] = 0;

	/* Unknown distance */
	m_ptr->cdis = 0;

	reset_target(m_ptr);

	m_ptr->nickname = 0;

	m_ptr->exp = 0;


	/* Your pet summons its pet. */
	//配下がモンスターを召喚(増殖含む)したとき呼ばれたモンスターの召喚元モンスターを記録する。召喚元モンスターが消えた時このモンスターも消える。
	///v1.1.11 サグメの機雷はidxを設定しない
	if (who > 0 && is_pet(&m_list[who]) && m_ptr->r_idx != MON_SAGUME_MINE)
	{
		mode |= PM_FORCE_PET;
		m_ptr->parent_m_idx = who;
	}
	else
	{
		m_ptr->parent_m_idx = 0;
	}

	/*:::カメレオンは生成時にすぐ変身し、フラグだけ残す*/
	if (r_ptr->flags7 & RF7_CHAMELEON)
	{
		choose_new_monster(c_ptr->m_idx, TRUE, 0);
		r_ptr = &r_info[m_ptr->r_idx];
		m_ptr->mflag2 |= MFLAG2_CHAMELEON;

		/* Hack - Set sub_align to neutral when the Chameleon Lord is generated as "GUARDIAN" */
		if ((r_ptr->flags1 & RF1_UNIQUE) && (who <= 0))
			m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
	}
	else if ((mode & PM_KAGE) && !(mode & PM_FORCE_PET))
	{
		m_ptr->ap_r_idx = MON_KAGE;
		m_ptr->mflag2 |= MFLAG2_KAGE;
	}

	if (mode & PM_NO_PET) m_ptr->mflag2 |= MFLAG2_NOPET;

	///mod140517 一時的存在フラグ
	if(mode & PM_EPHEMERA)
	{
		m_ptr->mflag |= MFLAG_EPHEMERA;
	}

	/* Not visible */
	m_ptr->ml = FALSE;

	//v1.1.73 八千慧買収工作
	if (flag_bribe_summon_monsters)
	{
		int price = r_ptr->level * r_ptr->level * 3;

		if (r_ptr->flags1 & RF1_UNIQUE) price *= 3;

		if (p_ptr->au >= price)
		{
			msg_format(_("%sはあなたの味方についた！", "%s becomes your ally!"),name);
			p_ptr->au -= price;
			p_ptr->redraw |= PR_GOLD;
			mode &= ~(PM_FORCE_ENEMY);
			mode |= PM_FORCE_PET;

		}
		else
			msg_print(_("工作資金が足りない!買収に失敗した!", "You don't have enough money! Recruiting failed."));


	}


	/* Pet? */
	if (mode & (PM_FORCE_PET | PM_EPHEMERA))
	{
		set_pet(m_ptr);
	}


	//敵対強制でないとき、条件を満たせば友好的になって出てくる処理
	//TODO:そのうち整頓する
	else if (!(mode & PM_FORCE_ENEMY))
	{
		//v1.1.32 一部の街では幻想郷モンスターが友好生成
		if (!who && !dun_level && !p_ptr->inside_arena && !p_ptr->inside_battle && !p_ptr->inside_quest
			&& (p_ptr->town_num == TOWN_HAKUREI || p_ptr->town_num == TOWN_MYOURENJI || p_ptr->town_num == TOWN_MORIYA || p_ptr->town_num == TOWN_HIGAN)
			&& (r_ptr->flags3 & RF3_GEN_MASK))
		{
			set_friendly(m_ptr);
		}
		else if (p_ptr->inside_quest == QUEST_YAKUZA_1)
		{
			; //やくざ戦争クエストでは指定以外の友好処理が行われない

		}
		else if (!p_ptr->inside_arena) //v1.1.51 アリーナ内では以下の友好処理がされないよう設定
		{
			/* Friendly? */
			/*:::モンスター生成時の友好設定*/
			//TODO:この処理適当に増やしすぎたし抜けも多そうだからいずれちゃんと見直したい

			//v1.1.93 強制友好召喚のときmonster_has_hostile_alignの判定をしないことにした
			if (mode & PM_FORCE_FRIENDLY)
			{
				if (cheat_hear) msg_print("(FORCE_FRIENDLY)");
				set_friendly(m_ptr);
			}
			///mod141231 街住みのモンスターは全て友好にした
			else if (((r_ptr->flags7 & RF7_FRIENDLY) || (r_ptr->flags3 & RF3_ANG_FRIENDLY) || (r_ptr->flags3 & RF3_GEN_FRIENDLY)
				|| (r_ptr->flags8 & (RF8_WILD_TOWN) && !(r_ptr->flags3 & (RF3_GEN_WARLIKE)) && !(r_ptr->flags3 & (RF3_ANG_WARLIKE)))
				|| you_are_human_align() && (r_ptr->flags3 & RF3_GEN_HUMAN) && p_ptr->inside_quest != QUEST_SENNIN && !(EXTRA_MODE && dun_level == 80) // Hack - 仙人クエスト除外
				|| is_friendly_idx(who)) && !(r_ptr->flags1 & RF1_QUESTOR))
			{
				if (!monster_has_hostile_align(NULL, 0, -1, r_ptr)) set_friendly(m_ptr);
			}
			else if (p_ptr->pclass == CLASS_NAZRIN && m_ptr->r_idx == MON_YOUKAI_MOUSE)  set_friendly(m_ptr);
			else if (p_ptr->pclass == CLASS_TEWI && m_ptr->r_idx == MON_YOUKAI_RABBIT2)  set_friendly(m_ptr);
			else if ((p_ptr->pclass == CLASS_RINGO || p_ptr->pclass == CLASS_SEIRAN || p_ptr->pclass == CLASS_SAGUME || p_ptr->pclass == CLASS_REISEN2 || p_ptr->pclass == CLASS_TOYOHIME || p_ptr->pclass == CLASS_YORIHIME)
				&& (r_ptr->flags3 & RF3_GEN_MOON))  set_friendly(m_ptr);
			else if (p_ptr->pclass == CLASS_SANAE && m_ptr->r_idx == MON_KANAKO && p_ptr->inside_quest != QUEST_KANASUWA) set_friendly(m_ptr);
			else if (p_ptr->pclass == CLASS_SANAE && m_ptr->r_idx == MON_SUWAKO && p_ptr->inside_quest != QUEST_KANASUWA) set_friendly(m_ptr);
			else if (p_ptr->pclass == CLASS_SATONO && m_ptr->r_idx == MON_MAI) set_friendly(m_ptr);
			else if (p_ptr->pclass == CLASS_MAI && m_ptr->r_idx == MON_SATONO) set_friendly(m_ptr);
			else if (p_ptr->pclass == CLASS_REMY)
			{
				if (m_ptr->r_idx == MON_MEIRIN)set_friendly(m_ptr);
				else if (m_ptr->r_idx == MON_SAKUYA)set_friendly(m_ptr);
				else if (m_ptr->r_idx == MON_PATCHOULI)set_friendly(m_ptr);
			}
			else if (p_ptr->pclass == CLASS_KAGUYA)
			{
				if (m_ptr->r_idx == MON_TEWI)set_friendly(m_ptr);
				else if (m_ptr->r_idx == MON_UDONGE)set_friendly(m_ptr);
				else if (m_ptr->r_idx == MON_EIRIN)set_friendly(m_ptr);
			}
			else if (p_ptr->pclass == CLASS_EIKI)
			{
				if (m_ptr->r_idx == MON_KOMACHI) set_friendly(m_ptr);
				else if (m_ptr->r_idx == MON_DEATH_1) set_friendly(m_ptr);
				else if (m_ptr->r_idx == MON_DEATH_2) set_friendly(m_ptr);
				else if (m_ptr->r_idx == MON_DEATH_3) set_friendly(m_ptr);

			}
			else if (p_ptr->pclass == CLASS_JUNKO && m_ptr->r_idx == MON_CLOWNPIECE)  set_friendly(m_ptr);
			else if (p_ptr->pclass == CLASS_RAN && m_ptr->r_idx == MON_CHEN)  set_friendly(m_ptr);
			else if (CHECK_FAIRY_TYPE == 4 && r_ptr->d_char == 'f' && !(r_ptr->flags1 & RF1_QUESTOR)) set_friendly(m_ptr);
			else if (CHECK_FAIRY_TYPE == 31 && r_ptr->flags3 & RF3_FAIRY && !(r_ptr->flags1 & RF1_QUESTOR)) set_friendly(m_ptr);
			else if (p_ptr->pclass == CLASS_CLOWNPIECE && m_ptr->r_idx == MON_JUNKO)  set_friendly(m_ptr);
			else if (p_ptr->pclass == CLASS_VFS_CLOWNPIECE && m_ptr->r_idx == MON_JUNKO)  set_friendly(m_ptr);
			else if (p_ptr->pclass == CLASS_HECATIA && (r_ptr->flags7 & RF7_FROM_HELL))  set_friendly(m_ptr);
			else if (p_ptr->pclass == CLASS_HECATIA && m_ptr->r_idx == MON_JUNKO)  set_friendly(m_ptr);
			else if (p_ptr->pclass == CLASS_JYOON && m_ptr->r_idx == MON_SHION_1)  set_friendly(m_ptr);
			//v1.1.65 妖夢と瓔花は幻想郷のGシンボルと仲が良いことにしておく
			else if ((p_ptr->pclass == CLASS_YOUMU || p_ptr->pclass == CLASS_EIKA) && (r_ptr->flags3 & RF3_GEN_MASK) && r_ptr->d_char == 'G')set_friendly(m_ptr);
			//v1.1.66 磨弓は埴輪と仲が良い
			else if (p_ptr->pclass == CLASS_MAYUMI && (r_ptr->flags3 & RF3_HANIWA))  set_friendly(m_ptr);
			//v1.1.71 早鬼と動物霊(勁牙組)
			else if (p_ptr->pclass == CLASS_SAKI && m_ptr->r_idx == MON_ANIMAL_G_KEIGA)  set_friendly(m_ptr);
			//v1.1.73 八千慧 鬼傑
			else if (p_ptr->pclass == CLASS_YACHIE && m_ptr->r_idx == MON_ANIMAL_G_KIKETSU)  set_friendly(m_ptr);
			//v1.1.74 袿姫
			else if (p_ptr->pclass == CLASS_KEIKI && (r_ptr->flags3 & RF3_HANIWA))  set_friendly(m_ptr);
			//v1.1.88 たかね
			else if(p_ptr->pclass == CLASS_TAKANE &&  m_ptr->r_idx == MON_YAMAWARO)  set_friendly(m_ptr);
			//v1.1.89 百々世
			else if (p_ptr->pclass == CLASS_MOMOYO || p_ptr->pclass == CLASS_TSUKASA)
			{
				if (m_ptr->r_idx == MON_MEGUMU) set_friendly(m_ptr);
			}
			else if (p_ptr->pclass == CLASS_MEGUMU)
			{
				if (m_ptr->r_idx == MON_MOMOYO || m_ptr->r_idx == MON_TSUKASA) set_friendly(m_ptr);
			}
			else if (p_ptr->pclass == CLASS_CHIMATA)
			{
				if (m_ptr->r_idx == MON_MOMOYO || m_ptr->r_idx == MON_TSUKASA || p_ptr->pclass == CLASS_MEGUMU) set_friendly(m_ptr);
			}



			//v1.1.59 針妙丸性格「意外に顔が広い」 WANTEDとQUESTOR以外の東方ユニークが常に友好
			if (is_special_seikaku(SEIKAKU_SPECIAL_SHINMYOUMARU))
			{
				if (is_gen_unique(m_ptr->r_idx) && !(r_info[m_ptr->r_idx].flags1 & RF1_QUESTOR) && !(r_info[m_ptr->r_idx].flags3 & RF3_WANTED))
					set_friendly(m_ptr);
			}

		}
	}

	/* Assume no sleeping */
	m_ptr->mtimed[MTIMED_CSLEEP] = 0;

	/*:::寝てるかどうか設定？寝てるのには度合いがあるらしい*/
	/* Enforce sleeping if needed */
	///mod150907 夢の世界では敵が全部起きている
	if ((mode & PM_ALLOW_SLEEP) && r_ptr->sleep && !ironman_nightmare && !(d_info[dungeon_type].flags1 & DF1_DREAM_WORLD))
	{
		int val = r_ptr->sleep;

		/*:::Hack 無縁塚では敵がよく起きている*/
		if(!(dun_level > 0 && dungeon_type == DUNGEON_MUEN && one_in_(6)))
			(void)set_monster_csleep(c_ptr->m_idx, (val * 2) + randint1(val * 10));
	}
	/*:::以後、HPなどパラメータ設定*/
	/* Assign maximal hitpoints */
	if (r_ptr->flags1 & RF1_FORCE_MAXHP)
	{
		m_ptr->max_maxhp = maxroll(r_ptr->hdice, r_ptr->hside);
	}
	else
	{
		m_ptr->max_maxhp = damroll(r_ptr->hdice, r_ptr->hside);
	}

	/* Monsters have double hitpoints in Nightmare mode */
	if (ironman_nightmare)
	{
		u32b hp = m_ptr->max_maxhp * 2L;

		m_ptr->max_maxhp = (s16b)MIN(30000, hp);
	}
	else if(difficulty == DIFFICULTY_EASY) m_ptr->max_maxhp = MAX(1,m_ptr->max_maxhp/2);
	else if(difficulty == DIFFICULTY_NORMAL) m_ptr->max_maxhp = MAX(1,(m_ptr->max_maxhp/2 + m_ptr->max_maxhp/4));

	m_ptr->maxhp = m_ptr->max_maxhp;
#if 0
	/* And start out fully healthy */
	if (m_ptr->r_idx == MON_WOUNDED_BEAR)
		m_ptr->hp = m_ptr->maxhp / 2;
	else
#endif
	m_ptr->hp = m_ptr->maxhp;

	//ヘカーティア特殊処理
	if(r_idx >= MON_HECATIA1 && r_idx <= MON_HECATIA3)
	{
		m_ptr->hp = hecatia_hp[r_idx - MON_HECATIA1];

	}



	/* Extract the monster base speed */
	/*:::monster_race.speedを取得し、ユニークとアリーナ生成以外では少しランダムに増減させる*/
	m_ptr->mspeed = get_mspeed(r_ptr);

	if (mode & PM_HASTE) (void)set_monster_fast(c_ptr->m_idx, 100);

	/* Give a random starting energy */
	/*:::次の行動に要するゲームターン数を初期設定*/
	if (mode & PM_NO_ENERGY)
	{
		m_ptr->energy_need = 100;
	}
	else if (!ironman_nightmare)
	{
		m_ptr->energy_need = ENERGY_NEED() - (s16b)randint0(100);
	}
	else
	{
		/* Nightmare monsters are more prepared */
		m_ptr->energy_need = ENERGY_NEED() - (s16b)randint0(100) * 2;
	}

	/* Force monster to wait for player, unless in Nightmare mode */
	if ((r_ptr->flags1 & RF1_FORCE_SLEEP) && !ironman_nightmare)
	{
		/* Monster is still being nice */
		/*:::MFLAG_NICEが立ってるうちはモンスターは魔法やブレス？を撃たない*/
		m_ptr->mflag |= (MFLAG_NICE);

		/* Must repair monsters */
		repair_monsters = TRUE;
	}

	/*:::Hack - EXTRAモードでダンジョン生成完了後に生成された非ユニークモンスターはフラグが付き、倒しても経験値やアイテムが少ない*/
	if(EXTRA_MODE && character_dungeon && !(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2)
		&& !is_pet(m_ptr))
	{
		m_ptr->mflag |= (MFLAG_EX_RESTRICT);
	}


	/* Hack -- see "process_monsters()" */
	/*:::生成されたモンスターが今行動しているモンスターよりindexが前（このターンでこのモンスターより後に処理される）の場合、フラグを立ててそのモンスターの行動順を飛ばす？*/
	if (c_ptr->m_idx < hack_m_idx)
	{
		/* Monster is still being born */
		m_ptr->mflag |= (MFLAG_BORN);
	}


	if (r_ptr->flags7 & RF7_SELF_LD_MASK)
		p_ptr->update |= (PU_MON_LITE);
	else if ((r_ptr->flags7 & RF7_HAS_LD_MASK) && !MON_CSLEEP(m_ptr))
		p_ptr->update |= (PU_MON_LITE);

	/* Update the monster */
	/*:::モンスターが＠から感知できるかどうか判定し、必要に応じて再描画や思い出更新やエルドリッチホラー処理など*/
	update_mon(c_ptr->m_idx, TRUE);


	/* Count the monsters on the level */
	/*:::r_info[].cur_numを加算している。m_ptr->r_idxをそのまま使ったらカメレオンが変身先の種族になるので修正している*/
	/*:::ただしEPHEMERAフラグ持ちが召喚されるときは無視する*/
	if(!(m_ptr->mflag & MFLAG_EPHEMERA))
		real_r_ptr(m_ptr)->cur_num++;

	/*
	 * Memorize location of the unique monster in saved floors.
	 * A unique monster move from old saved floor.
	 */
	/*:::ユニーク・準ユニークのmonster_race.floor_idを書き換える*/
	if (character_dungeon && !(mode & PM_EPHEMERA) &&
	    ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_NAZGUL) || ( (r_ptr->flags7 & RF7_VARIABLE) && (r_ptr->flags7 & RF7_UNIQUE2))))
		real_r_ptr(m_ptr)->floor_id = p_ptr->floor_id;

	/* Hack -- Count the number of "reproducers" */
	if (r_ptr->flags2 & RF2_MULTIPLY) num_repro++;

	/* Hack -- Notice new multi-hued monsters */
	/*:::多色シンボル・シェイプチェンジの敵が生成されたとき、shimmer_monsterフラグを立てる？*/
	{
		monster_race *ap_r_ptr = &r_info[m_ptr->ap_r_idx];
		if (ap_r_ptr->flags1 & (RF1_ATTR_MULTI | RF1_SHAPECHANGER))
			shimmer_monsters = TRUE;
	}
	/*:::警告処理*/
	///system 警告処理実体
	if (p_ptr->warning && character_dungeon)
	{
		if (r_ptr->flags1 & RF1_UNIQUE && !is_pet(m_ptr))
		{
			cptr color;
			object_type *o_ptr;
			char o_name[MAX_NLEN];

			if (r_ptr->level > p_ptr->lev + 30)
#ifdef JP
				color = "黒く";
#else
				color = "black";
#endif
			else if (r_ptr->level > p_ptr->lev + 15)
#ifdef JP
				color = "紫色に";
#else
				color = "purple";
#endif
			else if (r_ptr->level > p_ptr->lev + 5)
#ifdef JP
				color = "ルビー色に";
#else
				color = "deep red";
#endif
			else if (r_ptr->level > p_ptr->lev - 5)
#ifdef JP
				color = "赤く";
#else
				color = "red";
#endif
			else if (r_ptr->level > p_ptr->lev - 15)
#ifdef JP
				color = "ピンク色に";
#else
				color = "pink";
#endif
			else
#ifdef JP
				color = "白く";
#else
				color = "white";
#endif

			o_ptr = choose_warning_item();
			if (o_ptr)
			{
				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
				msg_format("%sは%s光った。", o_name, color);
#else
				msg_format("%s glows %s.", o_name, color);
#endif
			}
			/*:::自前警告用の処理もちゃんと書かれてた*/
			else
			{
				if(p_ptr->pclass == CLASS_ICHIRIN)
				{
					if (r_ptr->level > p_ptr->lev + 20)
						msg_print(_("雲山があなたに強敵の出現を知らせた。", "Unzan warns you of a powerful enemy."));
				}
				else
					msg_format(_("%s光る物が頭に浮かんだ。", "You have a vision of %s being."), color);
			}
		}
	}
	/*:::爆発のルーン処理*/
	if (is_explosive_rune_grid(c_ptr))
	{
		/* Break the ward */
		if (randint1(BREAK_MINOR_GLYPH) > r_ptr->level)
		{
			/* Describe observable breakage */
			if (c_ptr->info & CAVE_MARK)
			{
				/*:::特殊処理　チルノのみルーンの属性が違う*/
				int typ;
				if(p_ptr->pclass == CLASS_CIRNO)
				{
					typ = GF_ICE;
					msg_print(_("地面から氷柱が飛び出した！", "Ice pillar rises from the ground!"));
				}
				else
				{
					typ = GF_MANA;
					msg_print(_("ルーンが爆発した！", "The rune explodes!"));
				}
				project(0, 2, y, x, 2 * (p_ptr->lev + damroll(7, 7)), typ, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}
		}
		else
		{
#ifdef JP
msg_print("爆発のルーンは解除された。");
#else
			msg_print("An explosive rune was disarmed.");
#endif
		}

		/* Forget the rune */
		c_ptr->info &= ~(CAVE_MARK);

		/* Break the rune */
		c_ptr->info &= ~(CAVE_OBJECT);
		c_ptr->mimic = 0;

		note_spot(y, x);
		lite_spot(y, x);
	}

	//v1.1.86 常に一方向にしか進めない特殊モンスターの方向設定
	if (r_ptr->flags7 & RF7_FORCE_GO_STRAIGHT)
	{
		//クエスト「守矢の策動Ⅱ」内　常に下へ移動
		if (r_idx == MON_MARUTA && p_ptr->inside_quest == QUEST_MORIYA_2)
		{
			m_ptr->xtra_dir_param = 2;

		}

	}


	//v1.1.57 隠岐奈「秘神の暗躍弾幕」による処理
	if(p_ptr->pclass == CLASS_OKINA) process_pattern_attack(c_ptr->m_idx);



	/* Success */
	return (TRUE);
}


/*
 *  improved version of scatter() for place monster
 */

#define MON_SCAT_MAXD 10
/*:::指定位置からmas_dist離れてて、モンスターの種類を指定していればそのモンスターが入れる地形も考慮したランダムな場所を返す？*/
static bool mon_scatter(int r_idx, int *yp, int *xp, int y, int x, int max_dist)
{
	int place_x[MON_SCAT_MAXD];
	int place_y[MON_SCAT_MAXD];
	int num[MON_SCAT_MAXD];
	int i;
	int nx, ny;

	if (max_dist >= MON_SCAT_MAXD)
		return FALSE;

	for (i = 0; i < MON_SCAT_MAXD; i++)
		num[i] = 0;

	for (nx = x - max_dist; nx <= x + max_dist; nx++)
	{
		for (ny = y - max_dist; ny <= y + max_dist; ny++)
		{
			/* Ignore annoying locations */
			if (!in_bounds(ny, nx)) continue;

			/* Require "line of projection" */
			if (!projectable(y, x, ny, nx)) continue;

			if (r_idx > 0)
			{
				monster_race *r_ptr = &r_info[r_idx];

				/* Require empty space (if not ghostly) */
				if (!monster_can_enter(ny, nx, r_ptr, 0))
					continue;
			}
			else
			{
				/* Walls and Monsters block flow */
				if (!cave_empty_bold2(ny, nx)) continue;

				/* ... nor on the Pattern */
				if (pattern_tile(ny, nx)) continue;
			}

			i = distance(y, x, ny, nx);

			if (i > max_dist)
				continue;

			num[i]++;

			/* random swap */
			if (one_in_(num[i]))
			{
				place_x[i] = nx;
				place_y[i] = ny;
			}
		}
	}

	i = 0;
	while (i < MON_SCAT_MAXD && 0 == num[i])
		i++;
	if (i >= MON_SCAT_MAXD)
		return FALSE;

	*xp = place_x[i];
	*yp = place_y[i];

	return TRUE;
}


/*
 * Maximum size of a group of monsters
 */
#define GROUP_MAX	32


/*
 * Attempt to place a "group" of monsters around the given location
 */
static bool place_monster_group(int who, int y, int x, int r_idx, u32b mode)
{
	monster_race *r_ptr = &r_info[r_idx];

	int n, i;
	int total = 0, extra = 0;

	int hack_n = 0;
	int tmp_lev;


	byte hack_y[GROUP_MAX];
	byte hack_x[GROUP_MAX];


	/* Pick a group size */
	total = randint1(10);

	//v1.1.32 街の中ではモンスター集団出現の基準を街レベルにしてみる
	if(!dun_level && p_ptr->town_num)
		tmp_lev = wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].level;
	else
		tmp_lev = dun_level;
	/* Hard monsters, small groups */
	if ( r_ptr->level > tmp_lev)
	{
		extra = r_ptr->level - dun_level;
		extra = 0 - randint1(extra);
	}

	/* Easy monsters, large groups */
	else if (r_ptr->level < dun_level)
	{
		extra = dun_level - r_ptr->level;
		extra = randint1(extra);
	}

	/* Hack -- limit group reduction */
	if (extra > 9) extra = 9;

	/*:::現在レベルから低いモンスターの生成数は最高+9される。高いモンスターはそのぶん引かれる。*/
	/* Modify the group size */
	total += extra;

	/* Minimum size */
	if (total < 1) total = 1;

	/* Maximum size */
	if (total > GROUP_MAX) total = GROUP_MAX;


	/* Start on the monster */
	hack_n = 1;
	hack_x[0] = x;
	hack_y[0] = y;

	/* Puddle monsters, breadth first, up to total */
	/*:::groupのモンスターは繋がって生成される？*/
	for (n = 0; (n < hack_n) && (hack_n < total); n++)
	{
		/* Grab the location */
		int hx = hack_x[n];
		int hy = hack_y[n];

		/* Check each direction, up to total */
		for (i = 0; (i < 8) && (hack_n < total); i++)
		{
			int mx, my;

			scatter(&my, &mx, hy, hx, 4, 0);

			/* Walls and Monsters block flow */
			if (!cave_empty_bold2(my, mx)) continue;

			/* Attempt to place another monster */
			if (place_monster_one(who, my, mx, r_idx, mode))
			{
				/* Add it to the "hack" set */
				hack_y[hack_n] = my;
				hack_x[hack_n] = mx;
				hack_n++;
			}
		}
	}


	/* Success */
	return (TRUE);
}


/*
 * Hack -- help pick an escort type
 */
static int place_monster_idx = 0;
static int place_monster_m_idx = 0;

/*
 * Hack -- help pick an escort type
 */
/*:::ESCORTフラグモンスターの配下として適していればTRUE*/
static bool place_monster_okay(int r_idx)
{
	monster_race *r_ptr = &r_info[place_monster_idx];
	monster_type *m_ptr = &m_list[place_monster_m_idx];

	monster_race *z_ptr = &r_info[r_idx];

	/*:::Hack - アザトートの従者は専用*/
	if(place_monster_idx == MON_AZATHOTH)
	{
		if(r_idx == MON_LESSER_O_G || r_idx == MON_FLUTIST) return (TRUE);
		else return (FALSE);
	}

	/* Hack - Escorts have to have the same dungeon flag */
	if (mon_hook_dungeon(place_monster_idx) != mon_hook_dungeon(r_idx)) return (FALSE);

	/* Require similar "race" */
	if (z_ptr->d_char != r_ptr->d_char) return (FALSE);

	/* Skip more advanced monsters */
	if (z_ptr->level > r_ptr->level) return (FALSE);

	/* Skip unique monsters */
	if (z_ptr->flags1 & RF1_UNIQUE) return (FALSE);

	/*:::可変パラメータも配下としては出ないようにしておく*/
	if (z_ptr->flags7 & RF7_VARIABLE) return (FALSE);

	/* Paranoia -- Skip identical monsters */
	if (place_monster_idx == r_idx) return (FALSE);

	/* Skip different alignment */
	if (monster_has_hostile_align(m_ptr, 0, 0, z_ptr)) return FALSE;

	if (r_ptr->flags7 & RF7_FRIENDLY)
	{
		if (monster_has_hostile_align(NULL, 1, -1, z_ptr)) return FALSE;
	}

	if ((r_ptr->flags7 & RF7_CHAMELEON) && !(z_ptr->flags7 & RF7_CHAMELEON))
		return FALSE;

	/* Okay */
	return (TRUE);
}


/*
 * Attempt to place a monster of the given race at the given location
 *
 * Note that certain monsters are now marked as requiring "friends".
 * These monsters, if successfully placed, and if the "grp" parameter
 * is TRUE, will be surrounded by a "group" of identical monsters.
 *
 * Note that certain monsters are now marked as requiring an "escort",
 * which is a collection of monsters with similar "race" but lower level.
 *
 * Some monsters induce a fake "group" flag on their escorts.
 *
 * Note the "bizarre" use of non-recursion to prevent annoying output
 * when running a code profiler.
 *
 * Note the use of the new "monster allocation table" code to restrict
 * the "get_mon_num()" function to "legal" escort types.
 */
/*:::指定した座標に指定したインデックスのモンスターを配置する*/
bool place_monster_aux(int who, int y, int x, int r_idx, u32b mode)
{
	int             i;
	monster_race    *r_ptr = &r_info[r_idx];
	/*:::あやしい影？*/
	if (!(mode & PM_NO_KAGE) && one_in_(333))
		mode |= PM_KAGE;
	/* Place one monster, or fail */
	if (!place_monster_one(who, y, x, r_idx, mode)) return (FALSE);


	/* Require the "group" flag */
	if (!(mode & PM_ALLOW_GROUP)) return (TRUE);

	place_monster_m_idx = hack_m_idx_ii;

	/* Friends for certain monsters */
	if (r_ptr->flags1 & (RF1_FRIENDS))
	{
		/* Attempt to place a group */
		(void)place_monster_group(who, y, x, r_idx, mode);
	}

	/*:::配下のいるモンスターの処理*/
	/* Escorts for certain monsters */
	if (r_ptr->flags1 & (RF1_ESCORT))
	{
		/* Set the escort index */
		place_monster_idx = r_idx;

		/* Try to place several "escorts" */
		for (i = 0; i < 32; i++)
		{
			int nx, ny, z, d = 3;

			/* Pick a location */
			/*:::x,yからd程度離れた適当な場所を返す。　oやTの取り巻きはそれらがグループで出るがUの取り巻きが3マス超えて大量に出る理由がわからない*/
			scatter(&ny, &nx, y, x, d, 0);

			/* Require empty grids */
			if (!cave_empty_bold2(ny, nx)) continue;

			/* Prepare allocation table */
			/*:::詳細不明。モンスターの取り巻きとしてふさわしいraceを探してるらしいが・・:::*/
			get_mon_num_prep(place_monster_okay, get_monster_hook2(ny, nx));

			/* Pick a random race */
			/*:::取り巻きのレベルはモンスターのレベルに準じる*/
			z = get_mon_num(r_ptr->level);

			/* Handle failure */
			if (!z) break;

			//v1.1.64
			//Hack - 埴輪モンスターの護衛は埴輪だけにする。
			if ((r_ptr->flags3 & RF3_HANIWA) && !(r_info[z].flags3 & RF3_HANIWA)) continue;

			/* Place a single escort */
			(void)place_monster_one(place_monster_m_idx, ny, nx, z, mode);

			/* Place a "group" of escorts if needed */
			//v1.1.64 護衛が集団生成モンスターだったら大量に出てくるのを止めにした
			//if ((r_info[z].flags1 & RF1_FRIENDS) ||
			if ((r_ptr->flags1 & RF1_ESCORTS))
			{
				/* Place a group of monsters */
				(void)place_monster_group(place_monster_m_idx, ny, nx, z, mode);
			}
		}
	}

	/* Success */
	return (TRUE);
}


/*
 * Hack -- attempt to place a monster at the given location
 *
 * Attempt to find a monster appropriate to the "monster_level"
 */
bool place_monster(int y, int x, u32b mode)
{
	int r_idx;

	/* Prepare allocation table */
	get_mon_num_prep(get_monster_hook(), get_monster_hook2(y, x));

	/* Pick a monster */
	r_idx = get_mon_num(monster_level);

	/* Handle failure */
	if (!r_idx) return (FALSE);

	/* Attempt to place the monster */
	if (place_monster_aux(0, y, x, r_idx, mode)) return (TRUE);

	/* Oops */
	return (FALSE);
}


#ifdef MONSTER_HORDES
/*:::モンスターの大群生成*/
/*:::モンスター種類を決定し、一匹配置し、*/
bool alloc_horde(int y, int x)
{
	monster_race *r_ptr = NULL;
	int r_idx = 0;
	int m_idx;
	int attempts = 1000;
	int cy = y;
	int cx = x;

	/* Prepare allocation table */
	get_mon_num_prep(get_monster_hook(), get_monster_hook2(y, x));

	while (--attempts)
	{
		/* Pick a monster */
		r_idx = get_mon_num(monster_level);

		/* Handle failure */
		if (!r_idx) return (FALSE);

		r_ptr = &r_info[r_idx];
		/*:::ユニークとはぐれメタルは集団にならない*/
		if (r_ptr->flags1 & RF1_UNIQUE) continue;

		if (r_idx == MON_HAGURE) continue;
		break;
	}
	if (attempts < 1) return FALSE;

	attempts = 1000;

	while (--attempts)
	{
		/* Attempt to place the monster */
		/*:::まず中心モンスターを配置　modeにPM_ALLOW_SLEEPが設定されてないため起きているはず*/
		if (place_monster_aux(0, y, x, r_idx, 0L)) break;
	}

	if (attempts < 1) return FALSE;

	/*:::今配置されたばかりのモンスターのindexのはず*/
	m_idx = cave[y][x].m_idx;

	if (m_list[m_idx].mflag2 & MFLAG2_CHAMELEON) r_ptr = &r_info[m_list[m_idx].r_idx];
	summon_kin_type = r_ptr->d_char;

	for (attempts = randint1(10) + 5; attempts; attempts--)
	{
		scatter(&cy, &cx, y, x, 5, 0);

		(void)summon_specific(m_idx, cy, cx, dun_level + 5, SUMMON_KIN, PM_ALLOW_GROUP);

		y = cy;
		x = cx;
	}

	return TRUE;
}

#endif /* MONSTER_HORDES */


/*
 * Put the Guardian
 */
/*:::ダンジョンの主を配置する*/
///sys ダンジョンの主
///mod140412 EXダンジョンにモルゴスとアザトート配置
bool alloc_guardian(bool def_val)
{
	int guardian = d_info[dungeon_type].final_guardian;
	int oy;
	int ox;
	int try_num = 4000;

	//v2.0 オベロンが鉄国の主として出てこれるのは太歳星君を倒したあとのみ
	if (!CHECK_GUARDIAN_CAN_POP) return def_val;

	/*:::主が存在するダンジョンで最下階でまだ出現していないとき主生成を試みる*/
	if (guardian && (d_info[dungeon_type].maxdepth == dun_level) && (r_info[guardian].cur_num < r_info[guardian].max_num))
	{
		/* Find a good position */
		while (try_num)
		{
			/* Get a random spot */
			oy = randint1(cur_hgt - 4) + 2;
			ox = randint1(cur_wid - 4) + 2;

			/* Is it a good spot ? */
			/*:::そのモンスターが渡れる地形で他のモンスターや＠がおらず障害物になるようなものもないとき*/
			if (cave_empty_bold2(oy, ox) && monster_can_cross_terrain(cave[oy][ox].feat, &r_info[guardian], 0))
			{
				/* Place the guardian */
				if (place_monster_aux(0, oy, ox, guardian, (PM_ALLOW_GROUP | PM_NO_KAGE | PM_NO_PET))) return TRUE;
			}

			/* One less try */
			try_num--;
		}

		return FALSE;
	}
	else if(dungeon_type == DUNGEON_CHAOS)
	{
		int mon_idx;
		if(dun_level == 110 && r_info[MON_MORGOTH].cur_num < r_info[MON_MORGOTH].max_num
		|| dun_level == 120 && r_info[MON_AZATHOTH].cur_num < r_info[MON_AZATHOTH].max_num
		|| dun_level == 127 && r_info[MON_UNICORN_ORD].cur_num < r_info[MON_UNICORN_ORD].max_num)
		{
			if(dun_level == 110) mon_idx = MON_MORGOTH;
			else if(dun_level == 120) mon_idx = MON_AZATHOTH;
			else mon_idx = MON_UNICORN_ORD;
			while (try_num)
			{
				oy = randint1(cur_hgt - 4) + 2;
				ox = randint1(cur_wid - 4) + 2;
				if (cave_empty_bold2(oy, ox) && monster_can_cross_terrain(cave[oy][ox].feat, &r_info[mon_idx], 0))
				{
					if (place_monster_aux(0, oy, ox, mon_idx, (PM_ALLOW_GROUP | PM_NO_KAGE | PM_NO_PET))) return TRUE;
				}

			/* One less try */
			try_num--;
		}
		return FALSE;
		}
	}

	return def_val;
}

//v2.0.5 はたての調査で見つけたモンスターの配置
void alloc_hatate_search_mon()
{
	int try_num = 100;
	int ox, oy;
	int search_r_idx	= p_ptr->hatate_mon_search_ridx;
	int search_dungeon	= p_ptr->hatate_mon_search_dungeon / 1000;
	int search_level	= p_ptr->hatate_mon_search_dungeon % 1000;
	bool flag_success = FALSE;

	//特定フロアのみ
	if (!search_r_idx) return;
	if (search_dungeon != dungeon_type || search_level != dun_level) return;

	if (search_r_idx < 0 || search_r_idx >= max_r_idx)
	{
		msg_format(_("ERROR:p_ptr->hatate_mon_search_ridxの値がおかしい(%d)",
                    "ERROR: Weird value for p_ptr->hatate_mon_search_ridx (%d)"), search_r_idx);
		return;
	}

	//指定されたユニークモンスターを配置
	while (try_num)
	{
		/* Get a random spot */
		oy = randint1(cur_hgt - 4) + 2;
		ox = randint1(cur_wid - 4) + 2;

		if (cave_empty_bold2(oy, ox) && monster_can_cross_terrain(cave[oy][ox].feat, &r_info[search_r_idx], 0))
		{
			if (place_monster_aux(0, oy, ox, search_r_idx, (PM_ALLOW_GROUP | PM_NO_KAGE | PM_NO_PET))) flag_success = TRUE;
		}

		try_num--;
	}

	if(flag_success) msg_format(_("調査によるとここに%sがいるはずだ。",
                                "According to info you have, %s should be here."), r_name + r_info[search_r_idx].name);

	return;

}

/*
 * Attempt to allocate a random monster in the dungeon.
 *
 * Place the monster at least "dis" distance from the player.
 *
 * Use "slp" to choose the initial "sleep" status
 *
 * Use "monster_level" for the monster level
 */
/*:::モンスターを配置する。ダンジョンボスは必ず最初に配置判定がある*/
bool alloc_monster(int dis, u32b mode)
{
	int			y = 0, x = 0;
	int         attempts_left = 10000;

	/* Put the Guardian */
	if (alloc_guardian(FALSE)) return TRUE;

	/* Find a legal, distant, unoccupied, space */
	while (attempts_left--)
	{
		/* Pick a location */
		y = randint0(cur_hgt);
		x = randint0(cur_wid);

		/* Require empty floor grid (was "naked") */
		/*:::他のモンスターや＠がおらず障害物になるようなものもないとき　荒野では処理を簡略化*/
		if (dun_level)
		{
			if (!cave_empty_bold2(y, x)) continue;
		}
		else
		{
			if (!cave_empty_bold(y, x)) continue;
		}

		/* Accept far away grids */
		if (distance(y, x, py, px) > dis) break;
	}

	/*:::ここ、場合によっては警告が出まくるような気もするが*/
	if (!attempts_left)
	{
		if (cheat_xtra || cheat_hear)
		{
#ifdef JP
msg_print("警告！新たなモンスターを配置できません。小さい階ですか？");
#else
			msg_print("Warning! Could not allocate a new monster. Small level?");
#endif

		}

		return (FALSE);
	}
	/*:::この時点でモンスターを配置するための座標が決まった*/



#ifdef MONSTER_HORDES
	///mod141231 占いで不運になったとき大群生成率五倍
	if (randint1(5000) <= (dun_level * ((p_ptr->today_mon == FORTUNETELLER_BADLUCK) ? 5 : 1)))
	{
		if (alloc_horde(y, x))
		{
#ifdef JP
			if (cheat_hear) msg_format("モンスターの大群(%c)", summon_kin_type);
#else
			if (cheat_hear) msg_format("Monster horde (%c).", summon_kin_type);
#endif

			return (TRUE);
		}
	}
	else
	{
#endif /* MONSTER_HORDES */
		/* Attempt to place the monster, allow groups */
		if (place_monster(y, x, (mode | PM_ALLOW_GROUP))) return (TRUE);

#ifdef MONSTER_HORDES
	}
#endif /* MONSTER_HORDES */

	/* Nope */
	return (FALSE);
}




/*
 * Hack -- help decide if a monster race is "okay" to summon
 */
/*:::summon_specific()の中から、get_mon_num()用に召喚条件を満たすモンスターを判定する。*/
/*:::これを呼ぶ前に、summon_specific_typeにSUMMON_KINなどの召喚タイプ、*/
/*:::summon_unique_okayにユニーク召喚可否フラグ、summon_specific_whoに召喚者情報が格納されている*/
///sys summon_specificの召喚可能条件処理
static bool summon_specific_okay(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Hack - Only summon dungeon monsters */
	if (!mon_hook_dungeon(r_idx)) return (FALSE);

	///mod150103 ランダムユニークの抑止　以前出た時のシンボルやフラグでフィルタを通ってしまうことがあるため
	if(IS_RANDOM_UNIQUE_IDX(r_idx)) return (FALSE);

	/* Hack -- identify the summoning monster */
	if (summon_specific_who > 0)
	{
		monster_type *m_ptr = &m_list[summon_specific_who];
		monster_race *r_ptr_who = &r_info[m_ptr->r_idx];

		/* Do not summon enemies */

		/* Friendly vs. opposite aligned normal or pet */
		if (monster_has_hostile_align(m_ptr, 0, 0, r_ptr)) return FALSE;

		///mod140422
		/*::: -Hack- 勢力処理暫定として、幻想郷のユニークは外の者から召喚されないようにしておく*/
		if((r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_VARIABLE) && (r_ptr->flags3 & (RF3_GEN_MASK))
		&& !(r_ptr_who->flags3 & (RF3_GEN_MASK)))
			return FALSE;

	}
	/* Use the player's alignment */
	else if (summon_specific_who < 0)
	{
		/* Do not summon enemies of the pets */
		if (monster_has_hostile_align(NULL, 10, -10, r_ptr))
		{
			//if (!one_in_(ABS(p_ptr->align) / 2 + 1))
				if(!ignore_summon_align)return FALSE;
		}
	}


	/* Hack -- no specific type specified */
	if (!summon_specific_type) return (TRUE);

	if (!summon_unique_okay && ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_NAZGUL))) return FALSE;

	if ((summon_specific_who < 0) &&
	    ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_NAZGUL)) &&
	    monster_has_hostile_align(NULL, 10, -10, r_ptr))
		return FALSE;

	if ((r_ptr->flags7 & RF7_CHAMELEON) && (d_info[dungeon_type].flags1 & DF1_CHAMELEON)) return TRUE;

	return (summon_specific_aux(r_idx));
}


/*
 * Place a monster (of the specified "type") near the given
 * location.  Return TRUE if a monster was actually summoned.
 *
 * We will attempt to place the monster up to 10 times before giving up.
 *
 * Note: SUMMON_UNIQUE and SUMMON_AMBERITES will summon Unique's
 * Note: SUMMON_HI_UNDEAD and SUMMON_HI_DRAGON may summon Unique's
 * Note: None of the other summon codes will ever summon Unique's.
 *
 * This function has been changed.  We now take the "monster level"
 * of the summoning monster as a parameter, and use that, along with
 * the current dungeon level, to help determine the level of the
 * desired monster.  Note that this is an upper bound, and also
 * tends to "prefer" monsters of that level.  Currently, we use
 * the average of the dungeon and monster levels, and then add
 * five to allow slight increases in monster power.
 *
 * Note that we use the new "monster allocation table" creation code
 * to restrict the "get_mon_num()" function to the set of "legal"
 * monsters, making this function much faster and more reliable.
 *
 * Note that this function may not succeed, though this is very rare.
 */
/*:::召喚魔法全般の処理をする関数らしい。指定座標近辺に条件を満たすモンスターを1体配置することを試みる。*/
/*:::who:@の召喚なら-1,トラップなどで呼ばれたら0が入っている*/
/*:::type:召喚の種類。SUMMON_DEMON,SUMMON_KINなど*/
/*:::古代ドラゴン召喚などはこの関数を数回呼び出して複数召喚している*/
/*:::なお、SUMMON_KINを実行する前にはsummon_kin_typeにモンスターシンボルを設定する必要がある*/
bool summon_specific(int who, int y1, int x1, int lev, int type, u32b mode)
{
	int x, y, r_idx;

	if (p_ptr->inside_arena) return (FALSE);

	/*:::x1,y1から2マス離れた空いてる場所を返す*/
	/*:::宴会開催のとき範囲増やす*/
	if(mode & SUMMON_PHANTASMAGORIA)
	{
		if (!mon_scatter(0, &y, &x, y1, x1, 5)) return FALSE;
	}
	else
	{
		if (!mon_scatter(0, &y, &x, y1, x1, 2)) return FALSE;
	}
	/* Save the summoner */
	/*:::呼んだモンスターを保持*/
	summon_specific_who = who;

	/* Save the "summon" type */
	summon_specific_type = type;

	summon_unique_okay = (mode & PM_ALLOW_UNIQUE) ? TRUE : FALSE;

	/* Prepare allocation table */
	get_mon_num_prep(summon_specific_okay, get_monster_hook2(y, x));

	/* Pick a monster, using the level calculation */
	r_idx = get_mon_num((dun_level + lev) / 2 + 5);

	/* Handle failure */
	if (!r_idx)
	{
		summon_specific_type = 0;
		return (FALSE);
	}

	if ((type == SUMMON_BLUE_HORROR) || (type == SUMMON_DAWN)) mode |= PM_NO_KAGE;






	/* Attempt to place the monster (awake, allow groups) */
	if (!place_monster_aux(who, y, x, r_idx, mode))
	{
		summon_specific_type = 0;
		return (FALSE);
	}

	summon_specific_type = 0;
	/* Success */
	return (TRUE);
}

/* A "dangerous" function, creates a pet of the specified type */
/*:::特定種族のモンスターを生成する*/
bool summon_named_creature (int who, int oy, int ox, int r_idx, u32b mode)
{
	int x, y;

	/* Paranoia */
	/* if (!r_idx) return; */

	/* Prevent illegal monsters */
	if (r_idx >= max_r_idx) return FALSE;

	if (p_ptr->inside_arena) return FALSE;

	if (!mon_scatter(r_idx, &y, &x, oy, ox, 2)) return FALSE;

	/* Place it (allow groups) */
	return place_monster_aux(who, y, x, r_idx, (mode | PM_NO_KAGE));
}


/*
 * Let the given monster attempt to reproduce.
 *
 * Note that "reproduction" REQUIRES empty space.
 */
/*:::クローンモンスターか増殖モンスターの処理*/
bool multiply_monster(int m_idx, bool clone, u32b mode)
{
	monster_type	*m_ptr = &m_list[m_idx];

	int y, x;

	if (!mon_scatter(m_ptr->r_idx, &y, &x, m_ptr->fy, m_ptr->fx, 1))
		return FALSE;

	if (m_ptr->mflag2 & MFLAG2_NOPET) mode |= PM_NO_PET;

	/* Create a new monster (awake, no groups) */
	if (!place_monster_aux(m_idx, y, x, m_ptr->r_idx, (mode | PM_NO_KAGE | PM_MULTIPLY)))
		return FALSE;

	/* Hack -- Transfer "clone" flag */
	if (clone || (m_ptr->smart & SM_CLONED))
	{
		m_list[hack_m_idx_ii].smart |= SM_CLONED;
		m_list[hack_m_idx_ii].mflag2 |= MFLAG2_NOPET;
	}

	return TRUE;
}





/*
 * Dump a message describing a monster's reaction to damage
 *
 * Technically should attempt to treat "Beholder"'s as jelly's
 */
/*:::モンスターのリアクションを記述　詳細未読*/
///sys モンスターが痛がるメッセージ　変更予定　幻想郷の人型は別判定にしよう
///案：「余裕綽々だ」「まだまだ余裕のようだ」「少し焦ってきている」「服があちこち破けている」「ちょっと涙目だ」「%sの服はもうボロボロだ」「泣きそうになっている」
///sys mon モンスターのシンボル変更に対応要
///mod140107 少し編集　幻想ユニークを別処理にし、HP計算式を少し変えた
void message_pain(int m_idx, int dam)
{
	long oldhp, newhp, tmp;
	int percentage;

	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	char m_name[80];

	/* Get the monster name */
	monster_desc(m_name, m_ptr, 0);

	if(dam == 0) // Notice non-damage
	{
#ifdef JP
		msg_format("%^sはダメージを受けていない。", m_name);
#else
		msg_format("%^s is unharmed.", m_name);
#endif
		return;
	}

	/* Note -- subtle fix -CFT */
	newhp = (long)(m_ptr->hp);
	oldhp = newhp + (long)(dam);
	tmp = (newhp * 100L) / oldhp;
	percentage = (int)(tmp);

	///mod140816 赤蛮奇の頭　専用メッセージ
	if(m_ptr->r_idx == MON_BANKI_HEAD_1 || m_ptr->r_idx == MON_BANKI_HEAD_2)
	{

		if(percentage > 75) msg_format(_("%^sはまだ大丈夫だ。", "%^s is still fine."), m_name);
		else if(percentage > 50) msg_format(_("少し頭が痛い。", "Your head hurts a bit."), m_name);
		else if(percentage > 25) msg_format(_("%^sのダメージが大きい。", "%^s has taken severe damage."), m_name);
		else msg_format(_("頭痛がひどい。%^sはそろそろ落とされそうだ。",
                            "Your head hurts! %^s is about to drop down."), m_name);
	}
	///v1.1.24 ボート専用メッセージ
	else if(m_ptr->r_idx == MON_YOUR_BOAT || m_ptr->r_idx == MON_ANCIENT_SHIP)
	{

		if(percentage > 66) msg_format(_("%^sに傷がついた。", "%^s is damaged a bit."), m_name);
		else if(percentage > 33) msg_format(_("%^sは軋むような音を立てた。", "%^s makes a squeaking noise."), m_name);
		else msg_format(_("%^sが沈みそうだ！", "%^s is about to sink!"), m_name);
	}

	///mod140107 幻想郷の人型ユニークはメッセージを変える
	else if(is_gen_unique(m_ptr->r_idx) )
	{

		if(percentage > 95) msg_format(_("%^sは余裕綽々だ。", "%^s is calm and composed."), m_name);
		else if(percentage > 75) msg_format(_("%^sはまだまだ余裕のようだ。", "%^s still looks calm."), m_name);
		else if(percentage > 50) msg_format(_("%^sは少し焦ってきている。", "%^s looks a bit impatient."), m_name);
		else if(percentage > 35) msg_format(_("%^sの服があちこち破けている。", "The clothes %s is wearing are getting torn."), m_name);
		else if(percentage > 20) msg_format(_("%^sはちょっと涙目だ。", "%^s is getting teary-eyed."), m_name);
		else if(percentage > 10) msg_format(_("%^sの服はもうボロボロだ。", "The clothes %s is wearing are in tatters."), m_name);
		else msg_format(_("%^sは泣きそうになっている。", "%^s looks about to cry."), m_name);

	}

	else if(my_strchr(",ejmvwQ", r_ptr->d_char)) // Mushrooms, Eyes, Jellies, Molds, Vortices, Worms, Quylthulgs
	{
#ifdef JP
		if(percentage > 95) msg_format("%^sはほとんど気にとめていない。", m_name);
		else if(percentage > 75) msg_format("%^sはしり込みした。", m_name);
		else if(percentage > 50) msg_format("%^sは縮こまった。", m_name);
		else if(percentage > 35) msg_format("%^sは痛みに震えた。", m_name);
		else if(percentage > 20) msg_format("%^sは身もだえした。", m_name);
		else if(percentage > 10) msg_format("%^sは苦痛で身もだえした。", m_name);
		else msg_format("%^sはぐにゃぐにゃと痙攣した。", m_name);
#else
		if(percentage > 95) msg_format("%^s barely notices.", m_name);
		else if(percentage > 75) msg_format("%^s flinches.", m_name);
		else if(percentage > 50) msg_format("%^s squelches.", m_name);
		else if(percentage > 35) msg_format("%^s quivers in pain.", m_name);
		else if(percentage > 20) msg_format("%^s writhes about.", m_name);
		else if(percentage > 10) msg_format("%^s writhes in agony.", m_name);
		else msg_format("%^s jerks limply.", m_name);
#endif
	}

	else if(my_strchr("t", r_ptr->d_char)) // 天狗
	{
		if(percentage > 80) msg_format(_("%^sは不敵な笑みを浮かべている。", "%^s hovers with a fearless smile."), m_name);
		else if(percentage > 60) msg_format(_("%^sは悪態をついた。", "%^s curses."), m_name);
		else if(percentage > 40) msg_format(_("%^sはたじろいだ。", "%^s flinches."), m_name);
		else if(percentage > 15) msg_format(_("%^sはふらついた。", "%^s staggers about."), m_name);
		else msg_format(_("%^sは今にも倒れそうだ。", "%^s is about to collapse."), m_name);

	}
	else if(my_strchr("l", r_ptr->d_char)) // Fish
	{
		if(percentage > 95) msg_format(_("%^sはほとんど気にとめていない。", "%^s barely notices."), m_name);
		else if(percentage > 75) msg_format(_("%^sはしり込みした。", "%^s flinches."), m_name);
		else if(percentage > 50) msg_format(_("%^sは躊躇した。", "%^s hesitates."), m_name);
		else if(percentage > 35) msg_format(_("%^sは震えた。", "%^s shudders."), m_name);
		else if(percentage > 20) msg_format(_("%^sは身もだえした。", "%^s writhes about."), m_name);
		else if(percentage > 10) msg_format(_("%^sは弱弱しくうねった。", "%^s writhes weakly."), m_name);
		else msg_format(_("%^sは横になって痙攣した。", "%^s flops limply."), m_name);

	}

	else if(my_strchr("g#+<>", r_ptr->d_char)) // Golems, Walls, Doors, Stairs
	{
#ifdef JP
		if(percentage > 95) msg_format("%sは攻撃を気にとめていない。", m_name);
		else if(percentage > 75) msg_format("%sは攻撃に肩をすくめた。", m_name);
		else if(percentage > 50) msg_format("%^sは雷鳴のように吠えた。", m_name);
		else if(percentage > 35) msg_format("%^sは苦しげに吠えた。", m_name);
		else if(percentage > 20) msg_format("%^sはうめいた。", m_name);
		else if(percentage > 10) msg_format("%^sは躊躇した。", m_name);
		else msg_format("%^sはくしゃくしゃになった。", m_name);
#else
		if(percentage > 95) msg_format("%^s ignores the attack.", m_name);
		else if(percentage > 75) msg_format("%^s shrugs off the attack.", m_name);
		else if(percentage > 50) msg_format("%^s roars thunderously.", m_name);
		else if(percentage > 35) msg_format("%^s rumbles.", m_name);
		else if(percentage > 20) msg_format("%^s grunts.", m_name);
		else if(percentage > 10) msg_format("%^s hesitates.", m_name);
		else msg_format("%^s crumples.", m_name);
#endif
	}

	else if(my_strchr("JMR", r_ptr->d_char) ) // Snakes, Hydrae, Reptiles
	{
#ifdef JP
		if(percentage > 95) msg_format("%^sはほとんど気にとめていない。", m_name);
		else if(percentage > 75) msg_format("%^sはシーッと鳴いた。", m_name);
		else if(percentage > 50) msg_format("%^sは怒って頭を上げた。", m_name);
		else if(percentage > 35) msg_format("%^sは猛然と威嚇した。", m_name);
		else if(percentage > 20) msg_format("%^sは身もだえした。", m_name);
		else if(percentage > 10) msg_format("%^sは苦痛で身もだえした。", m_name);
		else msg_format("%^sはぐにゃぐにゃと痙攣した。", m_name);
#else
		if(percentage > 95) msg_format("%^s barely notices.", m_name);
		else if(percentage > 75) msg_format("%^s hisses.", m_name);
		else if(percentage > 50) msg_format("%^s rears up in anger.", m_name);
		else if(percentage > 35) msg_format("%^s hisses furiously.", m_name);
		else if(percentage > 20) msg_format("%^s writhes about.", m_name);
		else if(percentage > 10) msg_format("%^s writhes in agony.", m_name);
		else msg_format("%^s jerks limply.", m_name);
#endif
	}
	/*:::記号系の敵？*/
	else if( !isalpha(r_ptr->d_char) && !(r_ptr->flags3 & (RF3_ANIMAL | RF3_KWAI))) // Mimics
	{
		if(percentage > 75) msg_format(_("%^sは無反応だ。", "%^s does not react."), m_name);
		else if(percentage > 50) msg_format(_("%^sは耳障りな音を立てた。", "%^s lets out a jarring noise."), m_name);
		else if(percentage > 25) msg_format(_("%^sは弱々しく震えた。", "%^s shudders weakly."), m_name);
		else msg_format(_("%^sに亀裂が入った。", "%^s is cracked."), m_name);
	}

	else if(my_strchr("f", r_ptr->d_char))
	{
#ifdef JP
		if(percentage > 95) msg_format("%sは攻撃に肩をすくめた。", m_name);
		else if(percentage > 75) msg_format("%^sは吠えた。", m_name);
		else if(percentage > 50) msg_format("%^sは怒って吠えた。", m_name);
		else if(percentage > 35) msg_format("%^sは甲高い声で鳴いた。", m_name);
		else if(percentage > 20) msg_format("%^sは弱々しく鳴いた。", m_name);
		else if(percentage > 10) msg_format("%^sは苦痛にうめいた。", m_name);
		else msg_format("%sは哀れな鳴き声を出した。", m_name);
#else
		if(percentage > 95) msg_format("%^s shrugs off the attack.", m_name);
		else if(percentage > 75) msg_format("%^s roars.", m_name);
		else if(percentage > 50) msg_format("%^s growls angrily.", m_name);
		else if(percentage > 35) msg_format("%^s hisses with pain.", m_name);
		else if(percentage > 20) msg_format("%^s mewls in pain.", m_name);
		else if(percentage > 10) msg_format("%^s hisses in agony.", m_name);
		else msg_format("%^s mewls pitifully.", m_name);
#endif
	}

	else if(my_strchr("acI", r_ptr->d_char)) // Ants, Centipedes, Flies, Insects, Beetles, Spiders
	{
#ifdef JP
		if(percentage > 95) msg_format("%sは攻撃を気にとめていない。", m_name);
		else if(percentage > 75) msg_format("%^sはキーキー鳴いた。", m_name);
		else if(percentage > 50) msg_format("%^sはヨロヨロ逃げ回った。", m_name);
		else if(percentage > 35) msg_format("%^sはうるさく鳴いた。", m_name);
		else if(percentage > 20) msg_format("%^sは痛みに痙攣した。", m_name);
		else if(percentage > 10) msg_format("%^sは苦痛で痙攣した。", m_name);
		else msg_format("%^sはピクピクひきつった。", m_name);
#else
		if(percentage > 95)	msg_format("%^s ignores the attack.", m_name);
		else if(percentage > 75) msg_format("%^s chitters.", m_name);
		else if(percentage > 50) msg_format("%^s scuttles about.", m_name);
		else if(percentage > 35) msg_format("%^s twitters.", m_name);
		else if(percentage > 20) msg_format("%^s jerks in pain.", m_name);
		else if(percentage > 10) msg_format("%^s jerks in agony.", m_name);
		else msg_format("%^s twitches.", m_name);
#endif
	}

	else if(my_strchr("B", r_ptr->d_char)) // Birds
	{
#ifdef JP
		if(percentage > 95) msg_format("%^sはさえずった。", m_name);
		else if(percentage > 75) msg_format("%^sはピーピー鳴いた。", m_name);
		else if(percentage > 50) msg_format("%^sはギャーギャー鳴いた。", m_name);
		else if(percentage > 35) msg_format("%^sはギャーギャー鳴きわめいた。", m_name);
		else if(percentage > 20) msg_format("%^sは苦しそうな悲鳴を上げた。", m_name);
		else if(percentage > 10) msg_format("%^sはのたうった。", m_name);
		else msg_format("%^sは力なく一声鳴いた。", m_name);
#else
		if(percentage > 95)	msg_format("%^s chirps.", m_name);
		else if(percentage > 75) msg_format("%^s twitters.", m_name);
		else if(percentage > 50) msg_format("%^s squawks.", m_name);
		else if(percentage > 35) msg_format("%^s chatters.", m_name);
		else if(percentage > 20) msg_format("%^s jeers.", m_name);
		else if(percentage > 10) msg_format("%^s flutters about.", m_name);
		else msg_format("%^s squeaks helplessly.", m_name);
#endif
	}


	else if(my_strchr("s", r_ptr->d_char)) // Skeletons
	{
#ifdef JP
		if(percentage > 95) msg_format("%sは攻撃を気にとめていない。", m_name);
		else if(percentage > 75) msg_format("%sは攻撃に肩をすくめた。", m_name);
		else if(percentage > 50) msg_format("%^sはカタカタと笑った。", m_name);
		else if(percentage > 35) msg_format("%^sはよろめいた。", m_name);
		else if(percentage > 20) msg_format("%^sはガタガタとうなった。", m_name);
		else if(percentage > 10) msg_format("%^sにヒビが入りかけている。", m_name);
		else msg_format("%^sは崩れかけている。", m_name);
#else
		if(percentage > 95) msg_format("%^s ignores the attack.", m_name);
		else if(percentage > 75) msg_format("%^s shrugs off the attack.", m_name);
		else if(percentage > 50) msg_format("%^s rattles.", m_name);
		else if(percentage > 35) msg_format("%^s stumbles.", m_name);
		else if(percentage > 20) msg_format("%^s rattles.", m_name);
		else if(percentage > 10) msg_format("%^s staggers.", m_name);
		else msg_format("%^s clatters.", m_name);
#endif
	}

	else if(my_strchr("zH", r_ptr->d_char)) // Zombies
	{
#ifdef JP
		if(percentage > 95) msg_format("%sは攻撃を気にとめていない。", m_name);
		else if(percentage > 75) msg_format("%sは攻撃に肩をすくめた。", m_name);
		else if(percentage > 50) msg_format("%^sはうめいた。", m_name);
		else if(percentage > 35) msg_format("%sは苦しげにうめいた。", m_name);
		else if(percentage > 20) msg_format("%^sは躊躇した。", m_name);
		else if(percentage > 10) msg_format("%^sはうなった。", m_name);
		else msg_format("%^sはよろめいた。", m_name);
#else
		if(percentage > 95) msg_format("%^s ignores the attack.", m_name);
		else if(percentage > 75) msg_format("%^s shrugs off the attack.", m_name);
		else if(percentage > 50) msg_format("%^s groans.", m_name);
		else if(percentage > 35) msg_format("%^s moans.", m_name);
		else if(percentage > 20) msg_format("%^s hesitates.", m_name);
		else if(percentage > 10) msg_format("%^s grunts.", m_name);
		else msg_format("%^s staggers.", m_name);
#endif
	}

	else if(my_strchr("G", r_ptr->d_char)) // Ghosts
	{
#ifdef JP
		if(percentage > 95) msg_format("%^sは攻撃を気にとめていない。", m_name);
		else if(percentage > 75) msg_format("%^sはうなった。", m_name);
		else if(percentage > 50) msg_format("%^sはうめいた。", m_name);
		else if(percentage > 35) msg_format("%^sは悲しげな声を出した。", m_name);
		else if(percentage > 20) msg_format("%^sは吠えた。", m_name);
		else if(percentage > 10) msg_format("%sは弱々しくうめいた。", m_name);
		else msg_format("%^sは消えかけている。", m_name);
#else
		if(percentage > 95) msg_format("%^s ignores the attack.", m_name);
		else if(percentage > 75) msg_format("%^s shrugs off the attack.", m_name);
		else if(percentage > 50)  msg_format("%^s moans.", m_name);
		else if(percentage > 35) msg_format("%^s wails.", m_name);
		else if(percentage > 20) msg_format("%^s howls.", m_name);
		else if(percentage > 10) msg_format("%^s moans softly.", m_name);
		else msg_format("%^s starts to fade away.", m_name);
#endif
	}

	else if(my_strchr("CZ", r_ptr->d_char)) // Dogs and Hounds
	{
#ifdef JP
		if(percentage > 95) msg_format("%^sは攻撃に身をすくめた。", m_name);
		else if(percentage > 75) msg_format("%^sはうなった。", m_name);
		else if(percentage > 50) msg_format("%^sはキャンキャン吠えた。", m_name);
		else if(percentage > 35) msg_format("%^sは鳴きわめいた。", m_name);
		else if(percentage > 20) msg_format("%^sは尻尾を丸めた。", m_name);
		else if(percentage > 10) msg_format("%^sは悶えた。", m_name);
		else msg_format("%^sは弱々しく吠えた。", m_name);
#else
		if(percentage > 95) msg_format("%^s shrugs off the attack.", m_name);
		else if(percentage > 75) msg_format("%^s snarls with pain.", m_name);
		else if(percentage > 50) msg_format("%^s yelps in pain.", m_name);
		else if(percentage > 35) msg_format("%^s howls in pain.", m_name);
		else if(percentage > 20) msg_format("%^s howls in agony.", m_name);
		else if(percentage > 10) msg_format("%^s writhes in agony.", m_name);
		else msg_format("%^s yelps feebly.", m_name);
#endif
	}

	else if(my_strchr("Xbilqr", r_ptr->d_char)) // One type of creatures (ignore,squeal,shriek)
	{
#ifdef JP
		if(percentage > 95) msg_format("%^sは攻撃を気にとめていない。", m_name);
		else if(percentage > 75) msg_format("%^sはうなった。", m_name);
		else if(percentage > 50) msg_format("%^sは甲高い声を上げた。", m_name);
		else if(percentage > 35) msg_format("%^sは甲高い声を上げた。", m_name);
		else if(percentage > 20) msg_format("%^sは吠えた。", m_name);
		else if(percentage > 10) msg_format("%^sは悶えた。", m_name);
		else msg_format("%^sは弱々しく泣いた。", m_name);
#else
		if(percentage > 95) msg_format("%^s ignores the attack.", m_name);
		else if(percentage > 75) msg_format("%^s grunts with pain.", m_name);
		else if(percentage > 50) msg_format("%^s squeals in pain.", m_name);
		else if(percentage > 35) msg_format("%^s shrieks in pain.", m_name);
		else if(percentage > 20) msg_format("%^s shrieks in agony.", m_name);
		else if(percentage > 10) msg_format("%^s writhes in agony.", m_name);
		else msg_format("%^s cries out feebly.", m_name);
#endif
	}



	else if(my_strchr("FK", r_ptr->d_char)) // Fairy 河童も追加
	{
#ifdef JP
		if(percentage > 95) msg_format("%^sは攻撃を気にとめていない。", m_name);
		else if(percentage > 75) msg_format("%^sは騒いでいる。", m_name);
		else if(percentage > 50) msg_format("%^sはむくれている。", m_name);
		else if(percentage > 35) msg_format("%^sは悪態をついている。", m_name);
		else if(percentage > 20) msg_format("%^sは慌てている。", m_name);
		else if(percentage > 10) msg_format("%^sは悲鳴を上げた。", m_name);
		else msg_format("%^sは泣きそうになっている。", m_name);
#else
		if(percentage > 95) msg_format("%^s ignores the attack.", m_name);
		else if(percentage > 75) msg_format("%^s makes some noise.", m_name);
		else if(percentage > 50) msg_format("%^s puffs up.", m_name);
		else if(percentage > 35) msg_format("%^s grumbles.", m_name);
		else if(percentage > 20) msg_format("%^s looks to be in a hurry.", m_name);
		else if(percentage > 10) msg_format("%^s screams.", m_name);
		else msg_format("%^s looks about to cry.", m_name);
#endif
	}


	else if(my_strchr("duDLUWR", r_ptr->d_char) || (r_ptr->flags3 & (RF3_ANIMAL | RF3_KWAI))) // Dragons, Demons, High Undead
	{
#ifdef JP
		if(percentage > 95) msg_format("%sは攻撃を気にとめていない。", m_name);
		else if(percentage > 75) msg_format("%^sはしり込みした。", m_name);
		else if(percentage > 50) msg_format("%^sは痛みでシーッと鳴いた。", m_name);
		else if(percentage > 35) msg_format("%^sは痛みでうなった。", m_name);
		else if(percentage > 20) msg_format("%^sは痛みに吠えた。", m_name);
		else if(percentage > 10) msg_format("%^sは苦しげに叫んだ。", m_name);
		else msg_format("%^sは弱々しくうなった。", m_name);
#else
		if(percentage > 95) msg_format("%^s ignores the attack.", m_name);
		else if(percentage > 75) msg_format("%^s flinches.", m_name);
		else if(percentage > 50) msg_format("%^s hisses in pain.", m_name);
		else if(percentage > 35) msg_format("%^s snarls with pain.", m_name);
		else if(percentage > 20) msg_format("%^s roars with pain.", m_name);
		else if(percentage > 10) msg_format("%^s gasps.", m_name);
		else msg_format("%^s snarls feebly.", m_name);
#endif
	}


	else // Another type of creatures (shrug,cry,scream)
	{
#ifdef JP
		///msg131214

		if(percentage > 95) msg_format("%^sは攻撃に肩をすくめた。", m_name);
		else if(percentage > 75) msg_format("%^sは攻撃に顔をしかめた。", m_name);
		else if(percentage > 50) msg_format("%^sは攻撃に悪態をついた。", m_name);
		else if(percentage > 35) msg_format("%^sの息が荒い。", m_name);
		else if(percentage > 20) msg_format("%^sは呻き声を上げた。", m_name);
		else if(percentage > 10) msg_format("%^sは攻撃によろめいた。", m_name);
		else msg_format("%^sは今にも倒れそうだ。", m_name);

		/*
		if(percentage > 95) msg_format("%^sは攻撃に肩をすくめた。", m_name);
		else if(percentage > 75) msg_format("%^sは痛みでうなった。", m_name);
		else if(percentage > 50) msg_format("%^sは痛みで叫んだ。", m_name);
		else if(percentage > 35) msg_format("%^sは痛みで絶叫した。", m_name);
		else if(percentage > 20) msg_format("%^sは苦痛のあまり絶叫した。", m_name);
		else if(percentage > 10) msg_format("%^sは苦痛でもだえ苦しんだ。", m_name);
		else msg_format("%^sは弱々しく叫んだ。", m_name);
		*/
#else
		if(percentage > 95) msg_format("%^s shrugs off the attack.", m_name);
		else if(percentage > 75) msg_format("%^s frowns from the attack.", m_name);
		else if(percentage > 50) msg_format("%^s grumbles from the attack.", m_name);
		else if(percentage > 35) msg_format("%^s is breathing heavily.", m_name);
		else if(percentage > 20) msg_format("%^s moans.", m_name);
		else if(percentage > 10) msg_format("%^s staggers from the attack.", m_name);
		else msg_format("%^s looks about to collapse.", m_name);
#endif
	}
}




/*
 * Learn about an "observed" resistance.
 */
void update_smart_learn(int m_idx, int what)
{
	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];


	/* Not allowed to learn */
	if (!smart_learn) return;

	/* Too stupid to learn anything */
	if (r_ptr->flags2 & (RF2_STUPID)) return;
	/* Too smart to learn anything */
	if (r_ptr->flags2 & (RF2_SMART_EX)) return;


	/* Not intelligent, only learn sometimes */
	if (!(r_ptr->flags2 & (RF2_SMART)) && (randint0(100) < 50)) return;


	/* XXX XXX XXX */

	/* Analyze the knowledge */
	switch (what)
	{
	case DRS_ACID:
		if (p_ptr->resist_acid) m_ptr->smart |= (SM_RES_ACID);
		if (IS_OPPOSE_ACID()) m_ptr->smart |= (SM_OPP_ACID);
		if (p_ptr->immune_acid) m_ptr->smart |= (SM_IMM_ACID);
		break;

	case DRS_ELEC:
		if (p_ptr->resist_elec) m_ptr->smart |= (SM_RES_ELEC);
		if (IS_OPPOSE_ELEC()) m_ptr->smart |= (SM_OPP_ELEC);
		if (p_ptr->immune_elec) m_ptr->smart |= (SM_IMM_ELEC);
		break;

	case DRS_FIRE:
		if (p_ptr->resist_fire) m_ptr->smart |= (SM_RES_FIRE);
		if (IS_OPPOSE_FIRE()) m_ptr->smart |= (SM_OPP_FIRE);
		if (p_ptr->immune_fire) m_ptr->smart |= (SM_IMM_FIRE);
		break;

	case DRS_COLD:
		if (p_ptr->resist_cold) m_ptr->smart |= (SM_RES_COLD);
		if (IS_OPPOSE_COLD()) m_ptr->smart |= (SM_OPP_COLD);
		if (p_ptr->immune_cold){ m_ptr->smart |= (SM_IMM_COLD);}
		break;

	case DRS_POIS:
		if (p_ptr->resist_pois) m_ptr->smart |= (SM_RES_POIS);
		if (IS_OPPOSE_POIS()) m_ptr->smart |= (SM_OPP_POIS);
		break;


	case DRS_NETH:
		if (p_ptr->resist_neth) m_ptr->smart |= (SM_RES_NETH);
		break;

	case DRS_LITE:
		if (p_ptr->resist_lite) m_ptr->smart |= (SM_RES_LITE);
		break;

	case DRS_DARK:
		if (p_ptr->resist_dark) m_ptr->smart |= (SM_RES_DARK);
		break;

	case DRS_FEAR:
		if (p_ptr->resist_fear && !(IS_VULN_FEAR)) m_ptr->smart |= (SM_RES_FEAR);
		break;

	case DRS_CONF:
		if (p_ptr->resist_conf) m_ptr->smart |= (SM_RES_CONF);
		break;

	case DRS_CHAOS:
		if (p_ptr->resist_chaos) m_ptr->smart |= (SM_RES_CHAOS);
		break;

	case DRS_DISEN:
		if (p_ptr->resist_disen) m_ptr->smart |= (SM_RES_DISEN);
		break;

	case DRS_BLIND:
		if (p_ptr->resist_blind) m_ptr->smart |= (SM_RES_BLIND);
		break;
///pend res モンスターの因果混乱耐性学習部　いずれ他の耐性に書き換える
/*
	case DRS_NEXUS:
		if (p_ptr->resist_nexus) m_ptr->smart |= (SM_RES_NEXUS);
		break;
*/
	case DRS_SOUND:
		if (p_ptr->resist_sound) m_ptr->smart |= (SM_RES_SOUND);
		break;

	case DRS_SHARD:
		if (p_ptr->resist_shard) m_ptr->smart |= (SM_RES_SHARD);
		break;

	case DRS_FREE:
		if (p_ptr->free_act) m_ptr->smart |= (SM_IMM_FREE);
		break;
/*::: smartフラグが足りなくなったので削除　まあ大した影響はないだろう。
	case DRS_MANA:
		if (!p_ptr->msp) m_ptr->smart |= (SM_IMM_MANA);
		break;
*/
	case DRS_REFLECT:
		if (p_ptr->reflect) m_ptr-> smart |= (SM_IMM_REFLECT);
		break;

	case DRS_TIME:
		if (p_ptr->resist_time) m_ptr-> smart |= (SM_RES_TIME);
		break;
	case DRS_HOLY:
		if (p_ptr->resist_holy) m_ptr-> smart |= (SM_RES_HOLY);
		break;
	case DRS_WATER:
		if (p_ptr->resist_water) m_ptr-> smart |= (SM_RES_WATER);
		break;




	}
}


/*
 * Place the player in the dungeon XXX XXX
 */
bool player_place(int y, int x)
{
	int i;
	/* Paranoia XXX XXX */
	if (cave[y][x].m_idx != 0) return FALSE;

	/* Save player location */
	py = y;
	px = x;

	///mod160221 永久岩の中に出た場合場所変更するようにした　ウィザードモードや紫特技で広域マップ次元の扉を使った場合など
	///mod160312 「山脈」も再設定対象になってフリーズの原因になったため修正
	if(cave_have_flag_bold(py,px,FF_WALL) && cave_have_flag_bold(py,px,FF_PERMANENT) && !cave_have_flag_bold(py,px,FF_CAN_FLY))
	{
		//msg_print("chk");
		for(i=0;i<1000;i++)
		{
			py = randint1(cur_hgt-1);
			px = randint1(cur_wid-1);
			if(!in_bounds(py,px)) continue;
			if(cave[py][px].m_idx != 0) continue;
			if(cave_have_flag_bold(py,px,FF_WALL) && cave_have_flag_bold(py,px,FF_PERMANENT)) continue;
			break;
		}
	}

	/* Success */
	return TRUE;
}


/*
 * Drop all items carried by a monster
 */
/*:::モンスターが拾っていたアイテムを全て周辺に落とす処理*/
void monster_drop_carried_objects(monster_type *m_ptr)
{
	s16b this_o_idx, next_o_idx = 0;
	object_type forge;
	object_type *o_ptr;
	object_type *q_ptr;


	/* Drop objects being carried */
	for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Get local object */
		q_ptr = &forge;

		/* Copy the object */
		object_copy(q_ptr, o_ptr);

		/* Forget monster */
		q_ptr->held_m_idx = 0;

		/* Delete the object */
		delete_object_idx(this_o_idx);

		/* Drop it */
		(void)drop_near(q_ptr, -1, m_ptr->fy, m_ptr->fx);
	}

	/* Forget objects */
	m_ptr->hold_o_idx = 0;
}

/*:::幻想郷のいわゆる弾幕少女を判定　少しメッセージを変えたり一部のフィルタに使う*/
bool is_gen_unique(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	//チュパカブラとモケーレムベンベは例外的にFALSE
	if(r_idx == MON_CHUPACABRA) return FALSE;
	if(r_idx == MON_MOKELE) return FALSE;
	//V1.1.66 ライカ追加
	if (r_idx == MON_LAIKA) return FALSE;

	//霊夢と魔理沙はUNIQUEフラグを持たない特殊ユニークなのでここでTRUEを返す
	if(r_idx == MON_REIMU) return TRUE;
	if(r_idx == MON_MARISA) return TRUE;

	//ほか、幻想郷フラグをもつユニークは全てTRUEでそうでないものは全てFALSE
	if((r_ptr->flags3 & RF3_GEN_MASK) && (r_ptr->flags1 & RF1_UNIQUE)) return (TRUE);

	return (FALSE);


}
