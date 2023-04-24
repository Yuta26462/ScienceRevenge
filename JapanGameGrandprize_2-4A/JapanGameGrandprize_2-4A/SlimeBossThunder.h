#pragma once
#include"EnemyBulletBase.h"

#define LOCATION_DATA 60

class SlimeBossThunder :
    public EnemyBulletBase
{
    int image; //画像

    int x_speed; //Xのスピード
    int y_speed; //Yのスピード

    int color;

    bool left_move;
    int orientation_time;
    int data_switch;


    int old_radius[LOCATION_DATA];
    Location old_location[LOCATION_DATA];

public:

    //コンスタラクタ
    SlimeBossThunder(Location, Location);

    //デストラクタ
    ~SlimeBossThunder();

    //更新
    void Update() override;

    //描画
    void Draw() const override;
};