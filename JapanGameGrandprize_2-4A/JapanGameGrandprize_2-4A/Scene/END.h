#pragma once
#include "AbstractScene.h"
#include <string>

class End :
    public AbstractScene
{
private:

    enum class MENU
    {
        TITLE,
        EXIT,
        MENU_SIZE
    };

    const char* menu_items[static_cast<int>(MENU::MENU_SIZE)] = {
        "TITLE",
        "EXIT"
    };

    // 選択しているメニュー
    int select_menu;


    int menu_font;


    //選択SE用サウンドハンドル
    int select_se;
    //決定SE用サウンドハンドル
    int decision_se;
    
    //操作間隔時間
    int input_margin;
public:

    //コンストラクタ
    End();

    //デストラクタ
    ~End();

    //描画以外の更新を実行
    AbstractScene* Update() override;
   
    //描画に関することを実装
    void Draw() const override;
};