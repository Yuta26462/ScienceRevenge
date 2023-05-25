#include "GameClear.h"
#include "GameMain.h"
#include "../PadInput.h"
#include "DxLib.h"
#include "../Pouch.h"

GameClear::GameClear(short stage_num, unsigned int element_volume[PLAYER_ELEMENT], Pouch* pouch)
{
	title_font = CreateFontToHandle("Algerian", 100, 1, DX_FONTTYPE_ANTIALIASING_EDGE_8X8, -1, 8);
	menu_font = CreateFontToHandle("Algerian", 60, 1, DX_FONTTYPE_ANTIALIASING_EDGE_8X8, -1, 4);

	background_image = LoadGraph("Images/Scene/Gameclear.png");

	if ((background_music = LoadSoundMem("Sounds/BGM/game_clear.mp3")) == -1) {
		throw "Sounds/BGM/game_clear.mp3";
	}

	if ((enter_se = LoadSoundMem("Sounds/SE/enter.mp3")) == -1) {
		throw "Sounds/SE/enter.mp3";
	}

	if ((cursor_move_se = LoadSoundMem("Sounds/SE/cursor_move.mp3")) == -1)
	{
		throw "Sounds/SE/cursor_move.mp3";
	}

	this->stage_num = stage_num;

	PlaySoundMem(background_music, DX_PLAYTYPE_LOOP, FALSE);

	for (int i=0;i<PLAYER_ELEMENT;i++)
	{
		this->element_volume[i] = element_volume[i];	
	}

	this->pouch = pouch;
}

GameClear::~GameClear()
{
	StopSoundMem(background_music);
	DeleteSoundMem(background_music);
	DeleteSoundMem(enter_se);
	DeleteSoundMem(cursor_move_se);
	DeleteFontToHandle(title_font);
	DeleteFontToHandle(menu_font);
	DeleteGraph(background_image);
}

AbstractScene* GameClear::Update()
{
	if (PAD_INPUT::OnButton(XINPUT_BUTTON_A))
	{
		PlaySoundMem(enter_se, DX_PLAYTYPE_BACK, TRUE);
		while (CheckSoundMem(enter_se)) {}

		if (stage_num < 4)
		{
			return new GameMain(stage_num + 1, element_volume, pouch);
		}
		//else
		//{
		//	//return new GameEnd();
		//}
	}

	return this;
}

void GameClear::Draw() const
{
	DrawGraph(0, 0, background_image, FALSE);
	DrawStringToHandle(GetDrawCenterX("Game Clear", title_font), 150, "Game Clear", 0xFFFFFF,title_font, 0x000000);
	DrawStringToHandle(GetDrawCenterX("Next Stage", menu_font), 500, "Next Stage", 0x000000, menu_font, 0xFFFFFF);
	
}
