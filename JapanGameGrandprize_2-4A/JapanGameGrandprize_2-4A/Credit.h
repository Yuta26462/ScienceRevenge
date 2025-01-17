#pragma once
#include "Scene/AbstractScene.h"
class Credit :
    public AbstractScene
{
private:
    int display_time; //表示する時間
    int time; //時間
    unsigned int cr; //色

public:

    //コンストラクタ
    Credit();

    //デストラクタ
    ~Credit();

    //描画以外の更新を実行
    AbstractScene* Update();

    //描画に関することを実装
    void Draw() const;

};

