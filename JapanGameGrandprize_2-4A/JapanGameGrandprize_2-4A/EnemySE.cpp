#include "EnemySE.h"
#include "DxLib.h"


NomalEnemySE EnemySE::slime_se = { 0,0 };  //スライムSE
NomalEnemySE EnemySE::undead_se = { 0,0 }; //アンデットSE
NomalEnemySE EnemySE::harpy_se = { 0,0 }; //ハーピィSE
NomalEnemySE EnemySE::mage_se = { 0,0 }; //メイジSE
NomalEnemySE EnemySE::ghost_se = { 0,0 }; //ゴーストSE
NomalEnemySE EnemySE::wyvern_se = { 0,0 }; //ワイバーンSE
LastBossSE EnemySE::last_boss_se = {};
LastBossBarrierSE EnemySE::last_boss_barrier_se = {};
int EnemySE::down_se = 0;		//ダウン時SE
int EnemySE::explosion_se = 0;	//explosion被弾SE
int EnemySE::melt_se = 0;		//melt被弾SE
int EnemySE::poison_se = 0;		//poison被弾SE
int EnemySE::paralyze_se = 0;	//paralyze被弾SE

EnemySE::~EnemySE()
{
	
}

//-----------------------------------
//サウンドの読み込み
//-----------------------------------
void EnemySE::LoadSound()
{
	slime_se.attack = LoadSoundMem("Sounds/SE/Stage/EnemyAttack/slimeattack.wav");
	undead_se.attack = LoadSoundMem("Sounds/SE/Stage/EnemyAttack/undeadattack.wav");
	wyvern_se.attack = LoadSoundMem("Sounds/SE/Stage/EnemyAttack/wyvernbreath.wav");
	last_boss_se.punch = LoadSoundMem("Sounds/SE/Stage/EnemyAttack/LastBossPunch.wav");
	last_boss_se.special_attack[0] = LoadSoundMem("Sounds/SE/Stage/EnemyAttack/SpecialAttackCharge.wav");
	last_boss_se.special_attack[1] = LoadSoundMem("Sounds/SE/Stage/EnemyAttack/specialattack.wav");

	last_boss_barrier_se.appearance = LoadSoundMem("Sounds/SE/Stage/EnemyAttack/shield_appearance.wav");
	last_boss_barrier_se.breaked = LoadSoundMem("Sounds/SE/Stage/EnemyAttack/shield_break.wav");
	last_boss_barrier_se.hit = LoadSoundMem("Sounds/SE/Stage/EnemyAttack/shield_get_damage.wav");

	SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);
	explosion_se = LoadSoundMem("Sounds/SE/Stage/PlayerShot/explosion.wav",8);
	melt_se = LoadSoundMem("Sounds/SE/Stage/PlayerShot/melt.wav",8);
	poison_se = LoadSoundMem("Sounds/SE/Stage/PlayerShot/poison.wav",8);
	paralyze_se = LoadSoundMem("Sounds/SE/Stage/PlayerShot/paralyze.wav",8);
	SetCreateSoundDataType(DX_SOUNDDATATYPE_FILE);
}

//-----------------------------------
//サウンドの削除
//-----------------------------------
void EnemySE::DeleteSound()
{
	DeleteSoundMem(slime_se.attack);
	DeleteSoundMem(undead_se.attack);
	DeleteSoundMem(wyvern_se.attack);
	DeleteSoundMem(last_boss_se.punch);

	DeleteSoundMem(last_boss_barrier_se.appearance);
	DeleteSoundMem(last_boss_barrier_se.breaked);
	DeleteSoundMem(last_boss_barrier_se.hit);


	for (int i = 0; i < 2; i++)
	{
		DeleteSoundMem(last_boss_se.special_attack[i]);

	}

}

//-----------------------------------
//音量の設定
//-----------------------------------
void EnemySE::ChangeSoundVolume(const float volume)
{
	ChangeVolumeSoundMem(255 * (volume / 100), slime_se.attack);
	ChangeVolumeSoundMem(255 * (volume / 100), undead_se.attack);
	ChangeVolumeSoundMem(255 * (volume / 100), wyvern_se.attack);

	ChangeVolumeSoundMem(255, last_boss_barrier_se.breaked);
}

//-----------------------------------
//通常エネミーのSEの取得
//-----------------------------------
NomalEnemySE EnemySE::GetEnemySE(const ENEMY_KIND kind)
{
	NomalEnemySE ret = {};
	switch (kind)
	{
	case ENEMY_KIND::SLIME:
		ret = slime_se;
		break;
	case ENEMY_KIND::UNDEAD:
		ret = undead_se;
		break;
	case ENEMY_KIND::HARPY:
		break;
	case ENEMY_KIND::MAGE:
		break;
	case ENEMY_KIND::GHOST:
		break;
	case ENEMY_KIND::WYVERN:
		ret = wyvern_se;
		break;
	case ENEMY_KIND::SLIME_BOSS:
	case ENEMY_KIND::TORRENT:
	case ENEMY_KIND::KRAKEN:
	case ENEMY_KIND::DRAGON:
	case ENEMY_KIND::LAST_BOSS:
	case ENEMY_KIND::NONE:
		break;
	default:
		break;
	}

	return ret;
}

//-----------------------------------------------
// 被弾時のSEの取得
//-----------------------------------------------
int EnemySE::GetBulletSE(const ATTRIBUTE attribute)
{
	int return_se = 0;

	switch (attribute)
	{
	case ATTRIBUTE::NORMAL:
		;
		break;

	case ATTRIBUTE::EXPLOSION:
		return_se = explosion_se;
		break;

	case ATTRIBUTE::MELT:
		return_se = melt_se;
		break;

	case ATTRIBUTE::POISON:
		return_se = poison_se;
		break;

	case ATTRIBUTE::PARALYSIS:
		return_se = paralyze_se;
		break;

	case ATTRIBUTE::HEAL:
		;
		break;

	default:
		;
		break;
	}


	return return_se;
}

//ラスボスのSEの取得
LastBossSE EnemySE::GetLastBossSE()
{
	return last_boss_se;
}

LastBossBarrierSE EnemySE::GetBarrierSE() 
{
	return last_boss_barrier_se;
}
