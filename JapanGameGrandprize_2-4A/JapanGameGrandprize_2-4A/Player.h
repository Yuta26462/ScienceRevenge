#pragma once
#include "Define.h"
#include "BoxCollider.h"
#include "EfectBeam.h"
#include "Bullet.h"
#include "Stage/Stage.h"
#include "EnemyBase.h"
#include "ElementItem.h"
#include "Pouch.h"
#include "DxLib.h"
#include "EnumEnemyType.h"
#include "DeleteEffect.h"
#include <vector>

#define JUMP_INERTIA 0.2
#define WARK_INERTIA 0.5
#define HP_MAX 100.f
#define HP_BAR_WIDTH 286
#define HP_BAR_HEIGHT 22
#define FUEL_MAX 100.f
#define FUEL_BAR_HEIGHT 100

#define PLAYER_IMAGES 7


#define PLAYER_SPEED_X 5.0

#define BULLET_MAX 50


enum class PLAYER_STATE
{
	STOP = 0,
	MOVE_LEFT,
	MOVE_RIGHT,
	JUMP,
	FLY,
	DOWN,
	DEATH
};

enum class PLAYER_DEBUG_STATE
{
	STOP = 0,
	MOVE,
	FLY,
	DOWN,
	DEATH
};

class Player : public BoxCollider
{
private:

	DeleteEffect** effect;

	int animation;					//画像アニメーション用
	int image_count;				//画像の要素数用
	int* image;						//画像用変数
	int* jump_image;				//飛んでるときの画像
	int* attribute_images;			//属性の画像
	int hp_image;
	int effect_count;
	int hp_image_top;
	int image_size_x, image_size_y; //画像のサイズ
	int hp;							//体力
	int bullet_count;				//撃った弾の数
	int shoot_count;                //処理をカウントする


	float jump_power;				//ジャンプのパワー
	int jump_bottun_count;			//ジャンプボタンを入力している時間を数える	
	int not_jet_count;              //跳んでない時を数える
	float gravity_down;				//下降時の値
	float fuel;						//燃料
	float fly;                     //ホバリングの値
	float fly_power;               //ホバリングの力
	float speed_x;
	float old_x, old_y;				//壁とかに当たった時に元の位置に戻すための変数
	int select_count;

	int damage_count;				//無敵時間
	int flashing_count;				//点滅の間隔
	int i;                          //スイッチ内でのループ用
	int damage;                     //敵から受けたダメージの値
	int damage_by_second;			//敵からの持続ダメ値
	int damage_second;				//持続ダメの時間カウント用
	int damage_time;				//持続ダメのマックス時間

	bool fire_flg;					//火の持続ダメ中のフラグ
	bool fire_second_bool;			//火の持続ダメを食らっているときのフラグ
	bool damage_flg;				//ダメージを受けたかどうかのフラグ
	bool move_left;					//プレイヤーの向き true:左　false:右
	bool pouch_open;				//ポーチを開けている
	bool hit_stage;					//ステージのブロックに触れている
	bool boost;

	int bulletssound;//通常弾のBGM
	int flysound;//飛ぶ
	int healsound;//回復
	int deathsound;//死亡
	int open_menu;		//メニューを開く音
	int close_menu;		//メニューを閉じる音
	
	int heal_sound;



	ATTRIBUTE attribute[6];         //弾の属性
	const char* attribute_c[6];     //弾の属性の文字列
	int display_attribute;          //画面に表示させる属性

	PLAYER_STATE player_state;
	
	BulletBase** bullet;             //弾の配列

	Stage* stage;					 //ステージへのポインタ
	EfectBeam* beam;
	Pouch* pouch;					 //ポーチへのポインタ

	ElementItem** element;	//元素

	ChemicalFormulaParameter normal;

	ChemicalFormulaParameter* explosion;
	ChemicalFormulaParameter* poison;
	ChemicalFormulaParameter* melt;
	ChemicalFormulaParameter* pararysis;
	ChemicalFormulaParameter* heal;
	
	Heal_Effect effect_heal;
public:

	Player();
	Player(Stage*, unsigned int element_volume[PLAYER_ELEMENT], Pouch* pouch);
	~Player();
	void Draw()const;
	void PouchDraw()const;
	void PlayerUiDraw(float,float) const;
	void Update();
	void ElementUpdate();

	void RightMove();
	void LeftMove();
	void NotInputStick();
	void Jump();
	void Hovering();
	void Fly();
	void NotFly();
	void Shoot_Gun();
	void SortBullet(int);
	void SortEffect(int);
	void HpDamage(AttackResource);
	void Hp_Heal(int);
	bool HitBlock(const Stage*);

	BulletBase** GetBullet()const { return bullet; }
	PLAYER_STATE GetState() { return player_state; }
	bool GetIsPouchOpen()const { return pouch_open; }

	//プレイヤー所持元素取得関数
	ElementItem** GetPlayerElement() const { return element; }
	Pouch* GetPouch()const { return pouch; }
	


	//元素の量の設定
	void SetElementItem(class Item* item);
	//プレイヤーの向き
	bool GetMoveDirection();

	void SetExplosion(ChemicalFormulaParameter*);
	void SetPoison(ChemicalFormulaParameter*);
	void SetMelt(ChemicalFormulaParameter*);
	void SetPararysis(ChemicalFormulaParameter*);
	void SetHeal(ChemicalFormulaParameter*);

	void MoveAnimation();
	void HealAnimation(int x,int y);

#ifdef _DEBUG
	//更新(DotByDot)
	void Update(const PLAYER_DEBUG_STATE state);

	//描画(DotByDot)
	void DebugDraw();
#endif //_DEBUG
};