#include "DxLib.h"
#include "Torrent.h"


//-----------------------------------
//コンストラクタ
//-----------------------------------
Torrent::Torrent(Player* player)
{

}

//-----------------------------------
//デストラクタ
//-----------------------------------
Torrent::~Torrent()
{

}

//-----------------------------------
//更新
//-----------------------------------
void Torrent::Update()
{

}

//-----------------------------------
//攻撃
//-----------------------------------
AttackResource Torrent::Attack(const BoxCollider* collider)
{
	AttackResource ret = { 0,nullptr,0 }; //戻り値

	return ret;
}

//-----------------------------------
//プレイヤーの弾との当たり判定
//-----------------------------------
void Torrent::HitBullet(const BulletBase* bullet)
{

}

//-----------------------------------
//描画
//-----------------------------------
void Torrent::Draw() const
{

}

//-----------------------------------
//状態の取得
//-----------------------------------
TORRENT_STATE Torrent::GetState() const
{
	return state;
}