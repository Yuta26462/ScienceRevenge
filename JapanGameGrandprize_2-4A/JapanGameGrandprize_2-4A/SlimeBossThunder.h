#pragma once
#include"EnemyBulletBase.h"

#define LOCATION_DATA 60

class SlimeBossThunder :
    public EnemyBulletBase
{
    int image[8]; //画像
    int magic_circle_image;
    int image_type;
    int image_type_change_time;

    int x_speed; //Xのスピード
    int y_speed; //Yのスピード

    int color;

    bool left_move;
    int orientation_time;
    int data_switch;


    int old_radius[LOCATION_DATA];
    Location old_location[LOCATION_DATA];
    Location cloud_location;

public:

    //コンスタラクタ
    SlimeBossThunder(Location);

    //デストラクタ
    ~SlimeBossThunder();

    //更新
    void Update() override;

    //描画
    void Draw() const override;
    void Draw_Cloud()const;
};