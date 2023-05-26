#pragma once
#include "EnemyBase.h"
#include "EnemySE.h"
#include "BoxCollider.h"
#include "Player.h"

enum class TORRENT_ATTACK
{
    TACKLE = 0,     //タックル
    LEAF_CUTTER,    //葉っぱを飛ばす
    DROP_NUTS,      //木の実を落とす
    NONE
};

class Torrent :
    public EnemyBase
{
private:

    bool attack; //攻撃が当たったか
    bool tackle_end; //タックルが終わった
    int tackle_end_point; //タックルの終了地点
    int shot_rate; //魔法弾の発射レート
    int attack_time; //攻撃している時間
    int leaf_cutter_interval; //次の葉っぱを飛ばす攻撃に移る時間
    int drop_nuts_interval; //次の木の実を落とす攻撃に移る時間
    int spawn_interval; //木の実の生成する時間
    int animation; //アニメーション
    int image_argument; //画像の引数


    TORRENT_ATTACK attack_state; //攻撃の状態
private:

    //葉っぱの生成
    void CreateLeaf(Location);

    //木の実の生成
    void CreateNuts();

    //タックル攻撃
    void Tackle();

    //葉っぱを飛ばす攻撃
    void LeafCutter(Location);

    //木の実を落とす攻撃
    void DropNuts();

    // 攻撃していない
    void AttackNone();

    //アニメーション
    void Animation();

public:

    //コンストラクタ
    Torrent(Location);

    //デストラクタ
    ~Torrent();
    //更新
    void Update(const class Player* player, const class Stage* stage) override;

    //アイドル状態
    void Idol() override;

    //移動
    void Move(const Location player_location) override;

    //落下
    void Fall() override;

    //攻撃
    void  Attack(const Location) override;

    //攻撃が当たっているか
    AttackResource Hit() override;

    //死亡
    void Death() override;

    //プレイヤーの弾との当たり判定
    void HitBullet(const BulletBase* bullet) override;

    //描画
    void Draw() const override;

    //HPバーの描画
    void DrawHPBar(const int) const;

    //座標の取得
    Location GetLocation() const override;

#ifdef _DEBUG
    //更新(DotByDot)
    void Update(const ENEMY_STATE state) override;

    //描画(DotByDot)
    void DebugDraw() override;
#endif //_DEBUG
};