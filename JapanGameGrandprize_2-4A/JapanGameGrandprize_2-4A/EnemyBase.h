#pragma once
#include "Define.h"
#include "Bullet.h"
#include "ElementItem.h"
#include "EnumEnemyType.h"
#include "EnemyKind.h"
#include "BoxCollider.h"
#include "Stage/Stage.h"
#include <vector>
#include "EnemySE.h"


//各属性のドロップ数
#define FIRE_DROP 4
#define WATER_DROP 4
#define WIND_DROP 4
#define SOIL_DROP 4
#define THUNDER_DROP 7

//ダメージ倍率
#define WEAKNESS_DAMAGE 2.0f
#define RESISTANCE_DAMAGE 0.5f

//デバフの効果時間の倍率
#define WEAKNESS_DEBUFF 1.5f
#define	RESISTANCE_DEBUFF 0.5f

//毒ダメージを受けるまでのフレーム数
#define POISON_DAMAGE_FLAME 10

//麻痺状態の移動スピードの倍率
#define PARALYSIS_SPEED 0.5f

//落下速度
#define ENEMY_FALL_SPEED 1

//ダメージログの数
#define LOG_NUM 3

//ログの描画時間
#define LOG_TIME 60

//ログのY座標
#define LOG_Y 20

//当たり判定
struct HitMapChip
{
	bool hit; //当たったか
	MapChip* chip; //マップチップ
};

//ダメージ相性
enum class CONGENIALITY
{
	WEAKNESS = 0,	//弱点
	NORMAL,			//通常
	RESISTANCE,		//耐性
	INVALID,		//無効
};

//ステージブロックと当たった面
enum class STAGE_DIRECTION
{
	TOP,	//上面
	BOTTOM,	//下面
	LEFT,	//左面
	RIGHT	//右面
};


//エネミーの状態
enum class ENEMY_STATE
{
	IDOL,   //アイドル状態
	MOVE,   //移動
	FALL,	//落下
	ATTACK, //攻撃
	DEATH,  //死亡
};

struct DamageLog
{
	bool log; //ログの表示
	int damage; //ダメージ
	int time; //描画時間
	CONGENIALITY congeniality; //相性
};

class EnemyBase :
	public BoxCollider
{
public:

	//コンストラクタ
	EnemyBase();

	//デストラクタ
	~EnemyBase() {}

	//更新
	virtual void Update(const class Player* player, const class Stage* stage) = 0;

	//描画
	virtual void Draw() const = 0;

	//HPバーの描画
	virtual void DrawHPBar(const int) const;

	//ダメージログの描画
	virtual void DrawDamageLog() const;

	//ダメージログの更新
	virtual void UpdateDamageLog();

	//弱点属性のアイコン
	 void DrawWeaknessIcon() const;

	//アイドル状態
	virtual void Idol() = 0;

	//移動
	virtual void Move(const Location player_location) = 0;

	//落下
	virtual void Fall() = 0;

	//攻撃
	virtual void  Attack(const Location) = 0;

	//攻撃が当たっているか
	virtual AttackResource Hit() = 0;

	//ステージとの当たり判定
	HitMapChip HitStage(const class Stage* stage);

	//ステージのどの面と当たったを判断
	STAGE_DIRECTION HitDirection(const class MapChip* map_chip);

	//死亡
	virtual void Death() = 0;

	//プレイヤーの弾との当たり判定
	virtual void HitBullet(const BulletBase* bullet) = 0;
	void PlayHitBulletSound(ATTRIBUTE attribute) const;

	//ドロップする種類の量の取得
	int GetDropTypeVolume() const;

	//ドロップするアイテムの量の取得
	int GetDropVolume()const;

	//ドロップアイテムの取得
	ElementItem GetDropItem(int i) const;

	//エネミーの種類の取得
	ENEMY_KIND GetEnemyKind() const;

	//エネミーの状態の取得
	ENEMY_STATE GetState()const;

	//削除可能状態の取得
	bool GetCanDelete() const;

	//座標の取得
	virtual Location GetLocation() const = 0;

	//ログ用のフォントの作成
	static void CreateLogFont();

	//ログ用のフォントの削除
	static void DeleteLogFont();

	//弱点属性の情報のロード
	static void LoadWeakness();

	//弱点属性の情報の削除
	static void DeleteWeakness();

	//画像の削除
	static void DeleteImage();
#ifdef _DEBUG
	//更新(DotByDot)
	virtual void Update(const ENEMY_STATE state) = 0;

	//描画(DotByDot)
	virtual void DebugDraw() = 0;
#endif // _DEBUG
protected:

	bool can_delete; //削除フラグ
	bool left_move; //左に動いているかどうか
	bool poison;	//毒状態
	bool paralysis; //麻痺状態
	static std::vector<std::vector<int>> images; //画像
	static int* icon_images; //アイコン画像
	static int weakness_num[11]; //弱点の数
	static int magic_circle_image; //魔法陣の画像

	int hp;	//体力
	int speed; //移動速度
	int poison_time; //毒の効果時間
	int poison_damage; //毒のダメージ
	int paralysis_time; //麻痺の効果時間
	static int log_font[4]; //相性用のフォント(0:弱点,1:通常,2:耐性,3:弱点,)
	int drop_volume; //ドロップ量
	int drop_type_volume; //ドロップする種類の量
	ElementItem** drop_element; //ドロップ元素

	ENEMY_KIND kind; //エネミーの種類
	ENEMY_STATE state; //エネミーの状態
	static ATTRIBUTE* weakness[11]; //弱点
	DamageLog damage_log[LOG_NUM]; //ダメージログ
protected:

	//ダメージログの初期化
	void InitDamageLog();

	//HPが0かどうか判断(0になったらtrue)
	bool CheckHp();

	//画面外に出た
	bool ScreenOut();

	//毒状態の処理
	void Poison();

	//麻痺状態の処理
	void Paralysis();
};