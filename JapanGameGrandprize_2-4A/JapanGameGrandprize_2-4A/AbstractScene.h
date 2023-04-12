#pragma once

class AbstractScene
{
protected:

	//背景画像
	int background_image;

	//背景音楽
	int background_music;

	int title_font;

public:

	//デストラクタ
	virtual ~AbstractScene();

	//描画以外の更新を実行
	virtual AbstractScene* Update() = 0;

	//描画に関することを実装
	virtual void Draw() const = 0;

	int GetDrawCenterX(const char* string, int font_handle)const;
};