#pragma once
#include "EnemyKind.h"
#include "Define.h"

struct NomalEnemySE
{
    int move; //移動
    int attack; //攻撃
};

struct LastBossSE
{
    int punch;
    int special_attack[2];
};

class EnemySE
{
private:
   static NomalEnemySE slime_se;  //スライムSE
   static NomalEnemySE undead_se; //アンデットSE
   static NomalEnemySE harpy_se; //ハーピィSE
   static NomalEnemySE mage_se; //メイジSE
   static NomalEnemySE ghost_se; //ゴーストSE
   static NomalEnemySE wyvern_se; //ワイバーンSE
   static LastBossSE last_boss_se; //ラスボスSE

   static int down_se; //ダウン時SE

   static int explosion_se; //explosion被弾SE
   static int melt_se;      //melt被弾SE
   static int poison_se;    //poison被弾SE
   static int paralyze_se;  //paralyze被弾SE

private:
    EnemySE() = default;

public:

	~EnemySE();

    //サウンドの読み込み
    static void LoadSound();

    //サウンドの削除
    static void DeleteSound();

    //音量の設定
    static void ChangeSoundVolume(const float);

    //通常エネミーのSEの取得
    static NomalEnemySE GetEnemySE(const ENEMY_KIND);

    //ラスボスのSEの取得
    static LastBossSE GetLastBossSE();

    static int GetBulletSE(const ATTRIBUTE);
};

