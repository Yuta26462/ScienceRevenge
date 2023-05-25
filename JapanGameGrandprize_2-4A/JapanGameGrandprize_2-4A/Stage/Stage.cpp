#include "Stage.h"
#include "../CameraWork.h"
#include "DxLib.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "../Player.h"
#include "../CameraWork.h"
#include "Element/Stage_Element.h"
#include "../EnemyBase.h"

#define STAGE_NAME	"debugStage";
#define STAGE_NAME	"sample_stage2";
#define STAGE_NAME	"Stage01";
//#define STAGE_NAME  "Stage01_test";
//#define STAGE_NAME "Stage02";

//-----------------------------------
// コンストラクタ
//-----------------------------------
Stage::Stage(short stage_num)
{
	//ステージ番号をセット
	this->stage_num = stage_num;

	element = new Stage_Element(this);
	this->camera_work = camera_work;

	//背景画像枚数
	int image_num = 1;

	switch (this->stage_num)
	{
	case 0:
	case 1:
		image_num = 5;
		break;

	case 2: 
		image_num = 4;
		break;

	case 3:
		image_num = 3;
		break;

	default:
		break;
	}

	background_image.resize(image_num);

	//背景画像読み込み
	for (int i = 0; i < image_num; i++) {
		background_image[i] = 0;

		string dis_stage_graph;
		if (stage_num != 0)
		{
			dis_stage_graph = "Images/Scene/Stage/" + to_string(this->stage_num) + "/BackImage" + to_string(i + 1) + ".png";
		}
		else
		{
			dis_stage_graph = "Images/Scene/Stage/" + to_string(1) + "/BackImage" + to_string(i + 1) + ".png";

		}
		const char* dis_stage_graphc = dis_stage_graph.c_str();
		background_image[i] = LoadGraph(dis_stage_graphc);
		if (background_image[i] == -1) {
			/*background_image[i] = LoadGraph("Images/Scene/Stage/1/BackImage1.png");*/
			background_image[i] = 0;
			if (background_image[i] == -1) {
				throw dis_stage_graph;
			}
		}
	}

	for (int& c : backgraound_image_color)
	{
		c = 255;
	}

	backgraound_blend = 220;

	background_location = { 0.0f,0.0f };


	//スポーン地点に初期値をセット
	spawn_point = { MAP_CHIP_SIZE / 2, SCREEN_HEIGHT / 2 };

	if (LoadDivGraph("Images/Stage/map_chips.png", 50, 10, 5, CHIP_SIZE, CHIP_SIZE, block_images + 1) == -1)
	{
		throw "Images/Stage/map_chips.png";
	}

	switch (stage_num)
	{
	case 1:
		if (LoadDivGraph("Images/Stage/stage1_blocks.png", 10, 5, 2, CHIP_SIZE, CHIP_SIZE, stage1_block_images) == -1)
		{
			throw "Images/Stage/stage1_blocks.png";
		}
		break;

	case 2:
		if (LoadDivGraph("Images/Stage/stage2_blocks.png", 44, 4, 11, CHIP_SIZE, CHIP_SIZE, stage2_block_images + 1) == -1)
		{
			throw "Images/Stage/stage2_blocks.png";
		}
		break;

	default:
		if (LoadDivGraph("Images/Stage/stage1_blocks.png", 10, 5, 2, CHIP_SIZE, CHIP_SIZE, stage1_block_images) == -1)
		{
			throw "Images/Stage/stage1_blocks.png";
		}
		break;
	}
	

	//マップデータの読み込み
	LoadMap(stage_num);

	//フラグリセット
	is_halfway_point = false;


#ifdef _STAGE_BUILDER
	stage_builder = new StageBuilder();
#endif
}

//-----------------------------------
// デストラクタ
//-----------------------------------
Stage::~Stage()
{
	// ステージ背景画像を削除
	int image_num = background_image.size();
	for (int i = 0; i < image_num; i++)
	{
		DeleteGraph(background_image[i]);
	}

	//背景画像vectorの開放
	background_image.clear();
	background_image.shrink_to_fit();

	// マップチップ画像を削除
	for (int i = 0; i < 50; i++)
	{
		DeleteGraph(block_images[i]);
	}

	//vectorの解放

	// map_data内の各vectorを解放
	for (auto& inner_vec : map_data) {
		inner_vec.clear();
		inner_vec.shrink_to_fit();
	}



	// map_data自体を解放
	map_data.clear();
	map_data.shrink_to_fit();

	// MapChipオブジェクトを解放
	for (MapChip* chip : mapchip) {
		delete chip;
	}

	// mapchip自体を解放
	mapchip.clear();
	mapchip.shrink_to_fit();

	enemy_init_location.clear();
	enemy_init_location.shrink_to_fit();

	delete element;

	// 明るさをリセット
	SetDrawBright(255, 255, 255);

#ifdef _STAGE_BUILDER
	delete stage_builder;
#endif
}

//-----------------------------------
// 更新
//-----------------------------------
void Stage::Update(Player* player)
{
	ObjectCameraWork(player);

	Location player_location = player->GetLocation();

	//ElementにEnemyオブジェクトポインタ配列をセット
	if (enemy != nullptr) { element->SetEnemy(enemy); }

	// 中間地点との当たり判定
	if (abs(halfway_point.x - player_location.x) <= MAP_CHIP_SIZE
		&& abs(halfway_point.y - player_location.y) <= MAP_CHIP_SIZE) {
		is_halfway_point = true;
	}


	//当たり判定演算範囲
	struct DrawArea
	{
		float width;
		float height;
	} draw;

	draw = { SCREEN_WIDTH + CHIP_SIZE,SCREEN_HEIGHT + CHIP_SIZE };

	Location camera = CameraWork::GetCamera();

	for (auto& m : mapchip)
	{
		if (m == nullptr) continue;

		float x = m->GetLocation().x;
		float y = m->GetLocation().y;
		float w = m->GetArea().width;
		float h = m->GetArea().height;

		//ブロックの範囲を固定化
		w = MAP_CHIP_SIZE;
		h = MAP_CHIP_SIZE;

		// 画面内にあるMapChipオブジェクトだけUpdateする
		if (x + w < camera.x || camera.x + draw.width < x ||
			y + h < camera.y || camera.y + draw.height < y) continue;

		m->Update();
	}




	// カメラワークが固定されたらボス部屋を閉める
	if (camera_work != nullptr)
	{
		if (camera_work->GetCameraState() == CameraWork::STATE::FIXED &&
			SCREEN_WIDTH < player_location.x)
		{
			if (camera_work->GetCameraLock() == false)
			{
				camera_work->SetCameraLock(true);

				//float p_x = fmodf(player_location.x / MAP_CHIP_SIZE, SCREEN_WIDTH / CHIP_SIZE);
				//float wall_location = player_location.x / CHIP_SIZE - p_x + 2;

				//int map_height = map_data.size();
				//for (int i = 0; i < map_height; i++)
				//{
				//	if (map_data.at(i).at(static_cast<int>(wall_location)) < 1)
				//	{
				//		//画面端にブロックを設置
				//		//AddFixedMapChip(25, wall_location, static_cast<float>(i));
				//	}
				//}
			}
		}
	}

#ifdef _STAGE_BUILDER
	stage_builder->Update();
#endif

	element->Update(player);

}

void Stage::UpdateStageBackground(bool is_spawn_boss)
{
	//背景画像の更新
	background_location = CameraWork::GetCamera();
}

//-----------------------------------
// 描画
//-----------------------------------
void Stage::Draw()
{

	//マップチップ		描画

	//描画範囲
	struct DrawArea
	{
		float width;
		float height;
	} draw;

	draw = { SCREEN_WIDTH + CHIP_SIZE,SCREEN_HEIGHT + CHIP_SIZE };

	Location camera = CameraWork::GetCamera();

	for (auto& m : mapchip)
	{
		if (m == nullptr) continue;

		float x = m->GetLocation().x;
		float y = m->GetLocation().y;
		float w = m->GetArea().width;
		float h = m->GetArea().height;

		//ブロックの範囲を固定化
		w = MAP_CHIP_SIZE;
		h = MAP_CHIP_SIZE;

		// 画面内にあるMapChipオブジェクトだけ描画する
		if (x + w < camera.x || camera.x + draw.width < x || y + h < camera.y || camera.y + draw.height < y) continue;

		m->Draw();
	}

#ifdef _STAGE_BUILDER
	stage_builder->Draw();
#endif

	element->Draw();
}

void Stage::DrawStageBackground() const
{
	Location location = { 700,200 };
	switch (stage_num)
	{
	case 0:
	case 1:
		// 交差
		DrawGraphF(-fmodf(background_location.x * 0.2, SCREEN_WIDTH), 0, background_image[4], TRUE);
		DrawGraphF(-fmodf(background_location.x * 0.2, SCREEN_WIDTH) + SCREEN_WIDTH, 0, background_image[4], TRUE);

		DrawGraphF(-fmodf(background_location.x * 0.4, SCREEN_WIDTH), 0, background_image[3], TRUE);
		DrawGraphF(-fmodf(background_location.x * 0.4, SCREEN_WIDTH) + SCREEN_WIDTH, 0, background_image[3], TRUE);

		DrawGraphF(-fmodf(background_location.x * 0.6, SCREEN_WIDTH), 0, background_image[2], TRUE);
		DrawGraphF(-fmodf(background_location.x * 0.6, SCREEN_WIDTH) + SCREEN_WIDTH, 0, background_image[2], TRUE);

		DrawGraphF(-fmodf(background_location.x * 0.8, SCREEN_WIDTH), 0, background_image[1], TRUE);
		DrawGraphF(-fmodf(background_location.x * 0.8, SCREEN_WIDTH) + SCREEN_WIDTH, 0, background_image[1], TRUE);

		DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH), 0, background_image[0], TRUE);
		DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH) + SCREEN_WIDTH, 0, background_image[0], TRUE);
		break;
	case 2:
		//// 合成
		//DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH * 2), 0, background_image[0], TRUE);
		//DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH * 2) + SCREEN_WIDTH, 0, background_image[0], TRUE);
		//DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH * 2) + SCREEN_WIDTH * 2, 0, background_image[0], TRUE);

		//DrawGraphF(200.0f + object_camera_work.x, 100.0f + object_camera_work.y, background_image[1], TRUE);


		//DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH * 2), 0, background_image[2], TRUE);
		//DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH * 2) + SCREEN_WIDTH, 0, background_image[2], TRUE);
		//DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH * 2) + SCREEN_WIDTH * 2, 0, background_image[2], TRUE);

		DrawGraphF(-fmodf(background_location.x * 0.4, SCREEN_WIDTH), 0, background_image[3], TRUE);
		DrawGraphF(-fmodf(background_location.x * 0.4, SCREEN_WIDTH) + SCREEN_WIDTH, 0, background_image[3], TRUE);

		DrawGraphF(-fmodf(background_location.x * 0.6, SCREEN_WIDTH), 0, background_image[2], TRUE);
		DrawGraphF(-fmodf(background_location.x * 0.6, SCREEN_WIDTH) + SCREEN_WIDTH, 0, background_image[2], TRUE);

		DrawGraphF(-fmodf(background_location.x * 0.8, SCREEN_WIDTH), 0, background_image[1], TRUE);
		DrawGraphF(-fmodf(background_location.x * 0.8, SCREEN_WIDTH) + SCREEN_WIDTH, 0, background_image[1], TRUE);

		DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH), 0, background_image[0], TRUE);
		DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH) + SCREEN_WIDTH, 0, background_image[0], TRUE);

		break;
	case 3:
		/*SetDrawBright(100, 100, 100);
		DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH * 2), 0, background_image[0], TRUE);
		DrawTurnGraphF(-fmodf(background_location.x, SCREEN_WIDTH * 2) + SCREEN_WIDTH, 0, background_image[0], TRUE);
		DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH * 2) + SCREEN_WIDTH * 2, 0, background_image[0], TRUE);
		SetDrawBright(200, 200, 200);*/

		DrawGraphF(-fmodf(background_location.x * 0.6, SCREEN_WIDTH), 0, background_image[2], TRUE);
		DrawGraphF(-fmodf(background_location.x * 0.6, SCREEN_WIDTH) + SCREEN_WIDTH, 0, background_image[2], TRUE);

		DrawGraphF(-fmodf(background_location.x * 0.8, SCREEN_WIDTH), 0, background_image[1], TRUE);
		DrawGraphF(-fmodf(background_location.x * 0.8, SCREEN_WIDTH) + SCREEN_WIDTH, 0, background_image[1], TRUE);

		DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH), 0, background_image[0], TRUE);
		DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH) + SCREEN_WIDTH, 0, background_image[0], TRUE);

		break;

	case 4:
		DrawGraphF(-fmodf(background_location.x * 0.8, SCREEN_WIDTH), 0, background_image[0], TRUE);
		DrawGraphF(-fmodf(background_location.x * 0.8, SCREEN_WIDTH) + SCREEN_WIDTH, 0, background_image[0], TRUE);

		DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH), 0, background_image[0], TRUE);
		DrawGraphF(-fmodf(background_location.x, SCREEN_WIDTH) + SCREEN_WIDTH, 0, background_image[0], TRUE);
		
		break;

	default:
		break;
	}
}

void Stage::DrawObject() const
{
	
}

void Stage::ObjectCameraWork(Player* player)
{
	Location now_camera_work = CameraWork::GetCamera();

	if (now_camera_work.x != old_camera_work.x) {
		object_camera_work.x = now_camera_work.x - old_camera_work.x;
	
	}
	if (now_camera_work.y != old_camera_work.y) {
		object_camera_work.y -= now_camera_work.y - old_camera_work.y;
	}



	old_camera_work = CameraWork::GetCamera();
}

//-----------------------------------
// マップ読込み
//-----------------------------------
void Stage::LoadMap(short stage_num)
{

	//const char* stage_name = STAGE_NAME;

	char buf[37];
	sprintf_s(buf, sizeof(buf), "Data/Map_Data/Stage0%d.csv", stage_num);

	int FileHandle;

	if ((FileHandle = FileRead_open(buf)) == 0)
	{
		exit(1);
	}

	char str[2506];		//一行の長さ
	char* context;
	int i = 0, j = 0;

	while (FileRead_gets(str, sizeof(str), FileHandle) != -1)
	{

		char* tmp = strtok_s(str, ",", &context);

		map_data.push_back(std::vector<int>());

		while (tmp != NULL)
		{
			short chip_num = std::stoi(tmp);
			map_data[i].push_back(static_cast<int>(chip_num));

			//スポーン地点ID
			const short spawn_point_id = 777;
			if (chip_num == spawn_point_id) {
				spawn_point = { j * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2,
					i * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2
				};
			}

			//中間地点ID
			const short halfway_point_id = 100;
			if (i == halfway_point_id) {
				halfway_point = { j * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2,
					i * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2
				};
			}

			//エネミーのidの場合は、enemy_init_locationにPushしてスキップ
			if (enemy_id.find(chip_num) != enemy_id.end()) {
				enemy_init_location.push_back({ chip_num,
						j * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2,
						i * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2
					});
			}

			tmp = strtok_s(NULL, ",", &context);
			j++;
		}

		j = 0;
		i++;
	}

	FileRead_close(FileHandle);
}


/// <summary>
/// ステージの初期化
/// </summary>
void Stage::InitStage(void)
{

	//マップチップの描画情報をセット
	for (float y = 0; y < map_data.size(); y++)
	{
		for (float x = 0; x < map_data.at(y).size(); x++)
		{
			short i = map_data.at(y).at(x);
			if (i != 0 && i != -1)
			{
				if (element->GetElementID().find(i) != element->GetElementID().end()) {

					element->AddElement(i, {
						x * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2,
						y * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2
						}, { CHIP_SIZE,CHIP_SIZE });
				}
				else {
					// 固定ブロックの追加
					AddFixedMapChip(i, x, y);

				}
			}


		}
	}

}

void Stage::AddFixedMapChip(short id, float x, float y)
{

	if (stage_num == 2)
	{
		if (id < 44)
		{
			mapchip.push_back(new MapChip
			(&stage2_block_images[id],
				{
					x * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2,
					y * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2
				}, { CHIP_SIZE,CHIP_SIZE }));
		}
		else if (id == 99)
		{
			mapchip.push_back(new MapChip
			(&block_images[25],
				{
					x * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2,
					y * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2
				}, { CHIP_SIZE,CHIP_SIZE }));
		}
	}
	else
	{
		if (id == 99) { id = 25; }

		if (stage_id_base.find(id) != stage_id_base.end()) {
			int rand = GetRand(4);
			rand += 5;
			mapchip.push_back(new MapChip
			(&stage1_block_images[rand],
				{
					x * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2,
					y * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2
				}, { CHIP_SIZE,CHIP_SIZE }));
		}

		else if (stage_id_underground.find(id) != stage_id_underground.end()) {
			int rand = GetRand(4);
			mapchip.push_back(new MapChip
			(&stage1_block_images[rand],
				{
					x * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2,
					y * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2
				}, { CHIP_SIZE,CHIP_SIZE }));
		}
		//固定マップチップ
		else if (id < 50) {

			mapchip.push_back(new MapChip
			(&block_images[id],
				{
					x * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2,
					y * MAP_CHIP_SIZE + MAP_CHIP_SIZE / 2
				}, { CHIP_SIZE,CHIP_SIZE }));
		}
	}
}

void Stage::SetEnemy(EnemyBase** enemy)
{
	this->enemy = enemy;
	this->element->SetEnemy(enemy);
}

std::vector<MapChip*> Stage::GetMapChip() const
{

	std::vector<MapChip*>map_chip = mapchip;
	std::vector<Stage_Element_Base*>e_map_chip = GetElement_MapChip();
	map_chip.insert(map_chip.begin(), e_map_chip.begin(), e_map_chip.end());
	return map_chip;
}

std::vector<Stage_Element_Base*> Stage::GetElement_MapChip() const
{

	return	element->GetMapChip();
}

void Stage::SetElement()
{
	element->SetElementParameter();
}
