#pragma once
#include"EnemyBase.h"
#include"CameraWork.h"
#include"BoxCollider.h"

//#define DEBUG


enum class SLIME_ATTACK
{
	BEFORE_ATTACK,//攻撃する前
	AFTER_ATTACK,//攻撃した後
};

class EnemySlime :
	public EnemyBase
{
private:

	bool attack; //攻撃が当たったか

	SLIME_ATTACK slime_attack;

	int speed_x;
	int speed_y;
	int image_type;
	int image_change_time;
	int image_addition;
	int slime_angle;
	int wait_time;
	Location jump_distance;
	ENEMY_TYPE type;
private:
#ifdef _DEBUG
	ENEMY_STATE old_state; //一つ前の状態
	int attack_time; //攻撃の時間
	Location debug_location; //デバック用の座標
#endif // _DEBUG

public:

	EnemySlime(Location);
	~EnemySlime();

	//描画以外の更新を実行
	virtual void Update(const class Player* player, const class Stage* stage)override;

	//描画
	virtual void Draw()const override;

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

	virtual void HitBullet(const BulletBase* bullet)override;

	//座標の取得
	Location GetLocation() const override;

#ifdef _DEBUG
	//更新(DotByDot)
	void Update(const ENEMY_STATE state) override;

	//描画(DotByDot)
	void DebugDraw() override;
#endif //_DEBUG
};