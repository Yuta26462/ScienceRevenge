#include "Credit.h"
#include"DxLib.h"

//表示する時間
#define DISPLAY 240

//-----------------------------------
// コンストラクタ
//-----------------------------------
Credit::Credit()
{
	display_time = 0;
	time = 0;
	cr = GetColor(255, 255, 255); 
}


//-----------------------------------
// デストラクタ
//-----------------------------------
Credit::~Credit()
{

}

//-----------------------------------
// 更新
//-----------------------------------
AbstractScene* Credit::Update()
{
	if (++time % DISPLAY == 0)
	{
		if (++display_time > DISPLAY == 0)
		{
			return nullptr;
		}
	}
	return this;
}

//-----------------------------------
// 描画
//-----------------------------------
void Credit::Draw()const
{
	SetFontSize(64);                             //サイズを64に変更
	SetFontThickness(8);                         //太さを8に変更
	ChangeFont("ＭＳ 明朝");                     //種類をMS明朝に変更
	ChangeFontType(DX_FONTTYPE_ANTIALIASING);     //アンチエイリアスフォントに変更

	DrawString(400,100,"使用したSE  BGM\n",cr);

	SetFontSize(34);                             //サイズを35に変更

	DrawString(400,200,"フリー音楽素材 H/MIX GALLERY\n",cr);
	DrawString(400,250,"フリー効果音素材 くらげ工匠\n",cr); 
	DrawString(400,300,"無料効果音で遊ぼう！\n",cr); 
	DrawString(400,350,"TAM Music Factory\n",cr);
	DrawString(400,400,"DOVA-SYNDROME\n",cr);
	DrawString(400, 450,"効果音ラボ\n", cr); 
	
}