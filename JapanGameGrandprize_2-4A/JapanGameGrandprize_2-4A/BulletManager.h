#pragma once
#include "EnemyBulletBase.h"

class BulletManager
{
    // シングルトン
private:

    //コンストラクタ
    BulletManager();
    static BulletManager* instance; //インスタンス
public:

    //インスタンスの取得
    static BulletManager* GetInstance()
    {
        if (instance == nullptr)
        {
            instance = new BulletManager();
        }

        return instance;
    }
    // シングルトン　ここまで
private:

    EnemyBulletBase** enemy_bullets; //敵の弾
    EnemyBulletBase** enemy_nuts; //木の実
    int enemy_bullet_count; //弾の生成数
    int enemy_nuts_count; //木の実の生成数
    int enemy_bullet_max; //弾の生成可能数
    int enemy_nuts_max; //木の実の生成可能数

public:

    //デストラクタ
    ~BulletManager();

    //更新
    void Update(const class Stage* stage);

    //弾の生成
    void CreateEnemyBullet(class EnemyBulletBase* bullet);

    //弾の並べ替え
    void SortEnemyBullet(const int bullet_num);

    //弾の削除
    void DeleteEnemyBullet(const EnemyBulletBase* bullet);

    //木の実の生成
    void CreateEnemyNuts(class EnemyBulletBase* nuts);

    //木の実の並べ替え
    void SortEnemyNuts(const int nuts_num);

    //木の実の削除
    void DeleteEnemyNuts(const EnemyBulletBase* nuts);

    //攻撃が当たった
    AttackResource HitEnemyBullet(const int i);

    //攻撃が当たった
    AttackResource HitEnemyNuts(const int i);

    //描画
    void Draw() const;

    //弾の取得
    EnemyBulletBase** GetEnemyBullets() const;

    //弾の最大値の取得
    int EnemyGetBulletMax() const;

    //木の実の取得
    EnemyBulletBase** GetEnemyNuts() const;

    //木の実の最大値の取得
    int EnemyGetNutsMax() const;
};