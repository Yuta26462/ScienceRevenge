#pragma once
#include "AbstractScene.h"
#include "Player.h"
#include "EnemyBase.h"
#include "Stage/Stage.h"
#include "CameraWork.h"
#include"EnemyGhost.h"


class GameMain :
    public AbstractScene
{
private:

    //各クラスのオブジェクト
    Player* player;
    Stage* stage;
    CameraWork* camera_work;
    EnemyBase* enemy; //敵
    EnemyGhost* ghost; //敵（ゴースト)


    //操作間隔時間
    int input_margin;

public:
    //コンストラクタ
    GameMain();

    //デストラクタ
    ~GameMain();

    //更新
    AbstractScene* Update() override;

    //エネミーの更新処理
    void EnemyUpdate();

    //描画
    void Draw() const override;
};