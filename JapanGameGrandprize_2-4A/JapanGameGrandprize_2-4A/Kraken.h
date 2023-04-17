#pragma once
#include "EnemyBase.h"


enum class KRAKEN_STATE
{
	NORMAL,   //通常移動 
	NONE //何も行動しない。
};

enum class KRAKEN_ATTACK
{
	NONE = 0,  //攻撃をしていない。
	TENTACLE_ATTACK, //触手攻撃
	BREATH, //ブレス攻撃(水属性）
	HARD_ATTACK, //水の塊を落とす
	
};

class Kraken :
    public EnemyBase
{
private:

	KRAKEN_ATTACK attack_state;

public:

	//コンストラクタ
	Kraken(Location);

	//デストラクタ
	~Kraken();

	//描画以外の更新を実行
	void Update(const class Player* player, const class Stage* stage)override;

	//描画
	void Draw()const override;

	//アイドル状態
	void Idol() override;

	//移動
	void Move(const Location player_location) override;

	//接近攻撃（物理攻撃）時の動き
	void PhysicalMove(const Location player_location);

	//遠距離攻撃（水の塊を落とす）
	void AttackWater(const Location player_location);

	//落下
	void Fall() override;

	//攻撃
	void  Attack(const Location) override;

	//攻撃が当たっているか
	AttackResource Hit() override;

	//死亡
	void Death() override;

	//プレイヤーの弾丸との当たり判定
	void HitBullet(const BulletBase* bullet) override;

	//座標の取得
	Location GetLocation() const override;

#ifdef _DEBUG
	//更新(DotByDot)
	void Update(const ENEMY_STATE state) override;

	//描画(DotByDot)
	void DebugDraw() override;
#endif //_DEBUG
};

