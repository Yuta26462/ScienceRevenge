#include "Stage.h"
#include "DxLib.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "../Player.h"

#define STAGE_NAME	"debugStage";
#define STAGE_NAME	"sample_stage2";


//-----------------------------------
// コンストラクタ
//-----------------------------------
Stage::Stage()
{

	if (LoadDivGraph("Images/Stage/map_chips.png", 110, 10, 11, CHIP_SIZE, CHIP_SIZE, block_images + 1) == -1)
	{
		throw "Images/Stage/map_chips.png";
	}

	//マップデータの読み込み
	LoadMap();

	//マップチップの描画情報をセット
	for (float y = 0; y < map_data.size(); y++) 
	{
		for (float x = 0; x < map_data.at(0).size(); x++) 
		{
			int i = map_data.at(y).at(x);
			if (i != 0)
			{
				mapchip.push_back(new MapChip
				(&block_images[i],
					{
						x * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2,
						y * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2
					}, { CHIP_SIZE,CHIP_SIZE }));
			}
		}
	}

#ifdef _STAGE_BUILDER
	stage_builder = new StageBuilder();
#endif
}

//-----------------------------------
// デストラクタ
//-----------------------------------
Stage::~Stage()
{

	//マップチップの描画情報オブジェクトを削除
	for (int i = 0; i < mapchip.size(); i++)
	{
		delete mapchip.at(i);
	}

	mapchip.clear();
	mapchip.shrink_to_fit();

	//マップチップ画像を削除
	for (int i = 0; i < 100; i++) 
	{
		DeleteGraph(block_images[i]);
	}

#ifdef _STAGE_BUILDER
	delete stage_builder;
#endif
}

//-----------------------------------
// 更新
//-----------------------------------
void Stage::Update(Player* player)
{

	for (int i = 0; i < mapchip.size(); i++)
	{		
		mapchip.at(i)->Update(player);

	}

#ifdef _STAGE_BUILDER
	stage_builder->Update();
#endif
}

//-----------------------------------
// 描画
//-----------------------------------
void Stage::Draw()
{

	//マップチップ		描画
	for (int i = 0; i < mapchip.size(); i++)
	{
		mapchip.at(i)->Draw();
	}

#ifdef _STAGE_BUILDER
	stage_builder->Draw();
#endif

	DrawLine(500, 0, 500, 720, 0x0000FF);
	DrawLine(620, 0, 620, 720, 0xFF0000);
	
}

//-----------------------------------
// マップ読込み
//-----------------------------------
void Stage::LoadMap()
{
	const char* stage_name = STAGE_NAME;


	char buf[37];
	sprintf_s(buf, sizeof(buf), "Data/Map_Data/%s.csv", stage_name);

	int FileHandle;
	if ((FileHandle = FileRead_open(buf)) == 0) 
	{
		exit(1);
	}

	char str[900];		//一行の長さ
	char* context;
	int i = 0, j = 0;

	while (FileRead_gets(str, sizeof(str), FileHandle) != -1)
	{

		char* tmp = strtok_s(str, ",", &context);

		map_data.push_back(std::vector<int>());
		while (tmp != NULL) 
		{

			map_data[i].push_back(std::stoi(tmp));

			tmp = strtok_s(NULL, ",", &context);
			j++;
		}
		j = 0;
		i++;
	}

	FileRead_close(FileHandle);
}