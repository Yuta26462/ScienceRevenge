#include "BulletManager.h"
#include "Stage/Stage.h"
#include "CameraWork.h"

BulletManager* BulletManager::instance = nullptr;

//拡張量
#define ENEMY_BULLET_EXPANSION 10

//木の実の拡張量
#define NUTS_EXPANSION 5

//-----------------------------------
// コンストラクタ
//-----------------------------------
BulletManager::BulletManager()
{

    enemy_bullets = nullptr;
    enemy_nuts = nullptr;
    enemy_bullet_count = 0;
    enemy_nuts_count = 0;
    enemy_bullet_max = 0;
    enemy_nuts_max = 0;
}

//-----------------------------------
//デストラクタ
//-----------------------------------
BulletManager::~BulletManager()
{

    for (int i = 0; i < enemy_bullet_max; i++)
    {
        delete enemy_bullets[i];
    }
    delete[] enemy_bullets;

    for (int i = 0; i < enemy_nuts_max; i++)
    {
        delete enemy_nuts[i];
    }
    delete[] enemy_nuts;

    instance = nullptr;
}

//-----------------------------------
//更新
//-----------------------------------
void BulletManager::Update(const Stage* stage)
{

    for (int i = 0; i < enemy_bullet_count; i++)
    {
        if (enemy_bullets[i] == nullptr)
        {
            break;
        }

        enemy_bullets[i]->Update();

        if (enemy_bullets[i]->ScreenOut() || enemy_bullets[i]->HitStage(stage)) //画面外に出たか
        {
            delete enemy_bullets[i];
            enemy_bullets[i] = nullptr;

            SortEnemyBullet(i);
            enemy_bullet_count--;
            i--;
        }
    }

    for (int i = 0; i < enemy_nuts_count; i++)
    {
        if (enemy_nuts[i] == nullptr)
        {
            break;
        }

        enemy_nuts[i]->Update();

        if (enemy_nuts[i]->HitStage(stage)) //ステージにあたった
        {
            delete enemy_nuts[i];
            enemy_nuts[i] = nullptr;

            SortEnemyNuts(i);
            enemy_nuts_count--;
            i--;
        }
    }
}

//-----------------------------------
//弾の生成
//-----------------------------------
void BulletManager::CreateEnemyBullet(class EnemyBulletBase* bullet)
{

    if (enemy_bullet_max <= enemy_bullet_count)
    {
        if (enemy_bullets == nullptr)
        {
            //最大値の更新
            enemy_bullet_max += ENEMY_BULLET_EXPANSION;

            enemy_bullets = new EnemyBulletBase * [enemy_bullet_max];
            for (int i = 0; i < enemy_bullet_max; i++)
            {
                enemy_bullets[i] = nullptr;
            }
        }
        else
        {
            EnemyBulletBase** temporary_bullets; //避難用

            //避難
            temporary_bullets = enemy_bullets;

            //最大値の更新
            enemy_bullet_max += ENEMY_BULLET_EXPANSION;

            //弾の再生成
            enemy_bullets = new EnemyBulletBase * [enemy_bullet_max];

            //避難していた弾を戻す
            for (int i = 0; i < enemy_bullet_max; i++)
            {
                if (i < enemy_bullet_count)
                {
                    enemy_bullets[i] = temporary_bullets[i];
                }
                else
                {
                    enemy_bullets[i] = nullptr;
                }
            }
            delete[] temporary_bullets;
        }
    }
    //弾の生成
    enemy_bullets[enemy_bullet_count] = dynamic_cast<EnemyBulletBase*>(bullet);

    enemy_bullet_count++;
}

//-----------------------------------
//弾の並べ替え
//-----------------------------------
void BulletManager::SortEnemyBullet(const int bullet_num)
{

    //弾の中身をソートする
    for (int i = bullet_num + 1; i < enemy_bullet_max; i++)
    {
        if ((enemy_bullets[i] == nullptr))
        {
            break;
        }

        enemy_bullets[i - 1] = enemy_bullets[i];
        enemy_bullets[i] = nullptr;
    }
}

//-----------------------------------
//弾の削除
//-----------------------------------
void BulletManager::DeleteEnemyBullet(const EnemyBulletBase* bullet)
{

    for (int i = 0; i < enemy_bullet_max; i++)
    {
        if (enemy_bullets[i] == bullet)
        {
            delete enemy_bullets[i];
            enemy_bullets[i] = nullptr;

            SortEnemyBullet(i);
            enemy_bullet_count--;
            break;
        }
    }
}

//-----------------------------------
//木の実の生成
//-----------------------------------
void BulletManager::CreateEnemyNuts(class EnemyBulletBase* nuts)
{

    if (enemy_nuts_max <= enemy_nuts_count)
    {
        if (enemy_nuts == nullptr)
        {
            //最大値の更新
            enemy_nuts_max += ENEMY_BULLET_EXPANSION;

            enemy_nuts = new EnemyBulletBase * [enemy_nuts_max];
            for (int i = 0; i < enemy_nuts_max; i++)
            {
                enemy_nuts[i] = nullptr;
            }
        }
        else
        {
            EnemyBulletBase** temporary_nuts; //避難用

            //避難
            temporary_nuts = enemy_nuts;

            //最大値の更新
            enemy_nuts_max += ENEMY_BULLET_EXPANSION;

            //弾の再生成
            enemy_nuts = new EnemyBulletBase * [enemy_nuts_max];

           //避難していた弾を戻す
            for (int i = 0; i < enemy_nuts_max; i++)
            {
                if (i < enemy_nuts_count)
                {
                    if (enemy_nuts[i] != nullptr)
                    {
                        enemy_nuts[i] = temporary_nuts[i];
                    }
                }
                else
                {
                    enemy_nuts[i] = nullptr;
                }
            }
            delete[] temporary_nuts;
        }
    }
    //弾の生成
    enemy_nuts[enemy_nuts_count] = dynamic_cast<EnemyBulletBase*>(nuts);

    enemy_nuts_count++;
    PlayStreamSoundMem(EnemySE::GetTorrentSE().falling_nut_se, DX_PLAYTYPE_BACK);
}

//-----------------------------------
//木の実の並べ替え
//-----------------------------------
void BulletManager::SortEnemyNuts(const int nuts_num)
{

    //弾の中身をソートする
    for (int i = nuts_num + 1; i < enemy_nuts_max; i++)
    {
        if ((enemy_nuts[i] == nullptr))
        {
            break;
        }

        enemy_nuts[i - 1] = enemy_nuts[i];
        enemy_nuts[i] = nullptr;
    }
}

//-----------------------------------
//木の実の削除
//-----------------------------------
void BulletManager::DeleteEnemyNuts(const EnemyBulletBase* nuts)
{

    for (int i = 0; i < enemy_nuts_max; i++)
    {
        if (enemy_nuts[i] == nuts)
        {
            PlaySoundMem(EnemySE::GetTorrentSE().break_nut_se, DX_PLAYTYPE_BACK);
            delete enemy_nuts[i];
            enemy_nuts[i] = nullptr;

            SortEnemyNuts(i);
            enemy_nuts_count--;
            break;
        }
    }
}
//-----------------------------------
//攻撃が当たった
//-----------------------------------
AttackResource BulletManager::HitEnemyBullet(const int i)
{

    AttackResource ret = { 0,nullptr,0 }; //戻り値

    ENEMY_TYPE attack_type[1] = { enemy_bullets[i]->GetType() };
    ret.damage = enemy_bullets[i]->GetDamage();
    ret.type = attack_type;
    ret.type_count = 1;

    return ret;
}

//-----------------------------------
//攻撃が当たった
//-----------------------------------
AttackResource BulletManager::HitEnemyNuts(const int i)
{

    AttackResource ret = { 0,nullptr,0 }; //戻り値

    ENEMY_TYPE attack_type[1] = { enemy_nuts[i]->GetType() };
    ret.damage = enemy_nuts[i]->GetDamage();
    ret.type = attack_type;
    ret.type_count = 1;

    return ret;
}

//-----------------------------------
// 描画
//-----------------------------------
void BulletManager::Draw() const
{

    for (int i = 0; i < enemy_bullet_count; i++)
    {
        if (enemy_bullets[i] == nullptr)
        {
            break;
        }

        enemy_bullets[i]->Draw();
    }

    for (int i = 0; i < enemy_nuts_count; i++)
    {
        if (enemy_nuts[i] == nullptr)
        {
            break;
        }

        enemy_nuts[i]->Draw();
    }
}

//-----------------------------------
//弾の取得
//-----------------------------------
EnemyBulletBase** BulletManager::GetEnemyBullets() const
{

    return enemy_bullets;
}

//-----------------------------------
//弾の最大値の取得
//-----------------------------------
int BulletManager::EnemyGetBulletMax() const
{

    return enemy_bullet_max;
}

//-----------------------------------
//木の実の取得
//-----------------------------------
EnemyBulletBase** BulletManager::GetEnemyNuts() const
{

    return enemy_nuts;
}

//-----------------------------------
//木の実の最大値の取得
//-----------------------------------
int BulletManager::EnemyGetNutsMax() const
{

    return enemy_nuts_max;
}