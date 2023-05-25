#pragma once
#include "DxLib.h"
#include "BULLET.h"
#include "Define.h"
#include "SphereCollider.h"

#define PIXEL_MAX 4

class NormalBullet : public BulletBase
{
private:

	int Tick;
	float speed_x, speed_y[PIXEL_MAX];
	float dot_location_x[PIXEL_MAX], dot_location_y[PIXEL_MAX];
	Bullet_Effect Explosion[4];	//0:本弾　1:波動(発射時) 2:波動　3:軌跡
	Bullet_Effect Melt[2];		//0:本弾　1:軌跡
	Bullet_Effect Paralysis[2];	//0:本弾　1:軌跡
	Bullet_Effect Poison[2];	//0:本弾　1:軌跡
public:

	NormalBullet();
	NormalBullet(float, float,bool,ChemicalFormulaParameter*);
	~NormalBullet() {}

	void Draw() const override;
	void Update(const Stage*) override;

	bool NormalBulletEfect();
	
	void SetDeleteFlag(const Location);
	bool GetDelete_flg()const;								//trueをSetした後にエネミーはfalse待ちの状態にしてほしいです
														//falseが入った後にDeleteしてください
	int GetDamageParSecond() const override ;
};