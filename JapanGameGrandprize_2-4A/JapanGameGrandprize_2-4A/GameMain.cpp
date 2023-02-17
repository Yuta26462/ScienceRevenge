#include "GameMain.h"
#include "DxLib.h"
#include "Title.h"
#include "CameraWork.h"
#include "PadInput.h"
#include "Undead.h"
#include"EnemySlime.h"
#include "Item.h"
#include"EnemyGhost.h"

//-----------------------------------
// コンストラクタ
//-----------------------------------
GameMain::GameMain()
{
	stage = new Stage();
	player = new Player(stage);
	//enemy = new Undead(player);
	camera_work = new CameraWork(0, 0, player, stage);
	input_margin = 0;
	enemy = new EnemyBase * [3];
	enemy[0] = new EnemyGhost();
	enemy[1] = new Undead(player);
	enemy[2] = new EnemySlime();

}

//-----------------------------------
// デストラクタ
//-----------------------------------
GameMain::~GameMain()
{
	delete player;
	delete stage;
	delete enemy;
	delete camera_work;
}

//-----------------------------------
// 更新
//-----------------------------------
AbstractScene* GameMain::Update()
{
#ifdef _DEBUG
	//シーン切り替えテスト		デバック
	if (PAD_INPUT::OnButton(XINPUT_BUTTON_DPAD_RIGHT) && input_margin >= 30)
	{
		input_margin = 0;
		return new Title();
	}

	if (input_margin < 30)
	{
		input_margin++;
	}
#endif

	camera_work->Update();
	player->Update();
	stage->Update(player);

	EnemyUpdate();

	return this;
}

//-----------------------------------
// エネミーの更新処理
//-----------------------------------
void GameMain::EnemyUpdate()
{
	//Item** drop_item; //ドロップアイテム
	for (int i = 0; i < 3; i++)
	{
		enemy[i]->Update();

		switch (enemy[i]->GetEnemyKind())
		{
		case ENEMY_KIND::SLIME:		//スライム
		{
			EnemySlime* slime;
			slime = dynamic_cast<EnemySlime*>(enemy[i]);
			slime->HitPlayer(player);
			slime->AttackJudgement(player);
			break;
		}
		case ENEMY_KIND::UNDEAD:	//アンデット
		{
			Undead* undead;
			undead = dynamic_cast<Undead*>(enemy[i]);
			if (undead->GetState() == UNDEAD_STATE::ATTACK)
			{
				if (undead->HitBox(player))
				{

				}
			}
			break;
		}
		case ENEMY_KIND::HARPY:		//ハーピィ
		{
			break;
		}
		case ENEMY_KIND::MAGE:		//メイジ
		{
			break;
		}
		case ENEMY_KIND::GHOST:		//ゴースト
		{
			EnemyGhost* ghost;
			ghost = dynamic_cast<EnemyGhost*>(enemy[i]);
			ghost->GhostMove(player);
			break;
		}
		case ENEMY_KIND::WYVERN:	//ワイバーン
		{
			break;
		}
		case ENEMY_KIND::KING_SLIME://スライムキング
		{
			break;
		}
		case ENEMY_KIND::TORRENT:	//トレント
		{
			break;
		}
		case ENEMY_KIND::GARGOYLE:	//ガーゴイル
		{
			break;
		}
		case ENEMY_KIND::DRAGON:	//ドラゴン
		{
			break;
		}
		case ENEMY_KIND::END_BOSS:	//ラスボス
		{
			break;
		}
		case ENEMY_KIND::NONE:
			break;
		default:
			break;
		}
	}
}

//-----------------------------------
// 描画
//-----------------------------------
void GameMain::Draw()const
{
	//背景
	SetBackgroundColor(149, 249, 253);

	player->Draw();
	stage->Draw();
	for (int i = 0; i < 3; i++)
	{
		enemy[i]->Draw();
	}
}