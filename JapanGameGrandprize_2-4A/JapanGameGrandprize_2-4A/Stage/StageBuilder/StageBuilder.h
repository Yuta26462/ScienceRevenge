#pragma once
#include "Debug.h"
#include "../Define.h"
#include "KeyManager.h"
#include "../MapChip.h"
#include "../SphereCollider.h"
#include <vector>

#define MENU_NUM 3
#define MENU_MODE -1
#define BRUSH_MODE 0
#define MODULATION_MODE 1
#define SAVE_MODE 2

#define ARROW_NUM 16

using namespace std;

class StageBuilder
{
private:
	SphereCollider* mouse;//����̂�
	Location mouse_pos;
	//�u���b�N�摜
	int block_images[100];
	vector<MapChip*> map_chips;

	int mode;

	int menu_cursor;
	char arrow[ARROW_NUM];

public:
	//�R���X�g���N�^
	StageBuilder();
	//�f�X�g���N�^
	~StageBuilder();
	//�X�V
	void Update();
	//�`��
	void Draw()const;

	//���j���[���[�h�̍X�V
	void UpdateMenu();
	//�u���V���[�h�̍X�V
	void UpdateBrush();
	//���f�����[�V�������[�h�̍X�V
	void UpdateModulation();
	//�Z�[�u���[�h�̍X�V
	void UpdateSave();

	//�}�E�X�̍X�V
	void UpdateMouse();
	//���j���[���[�h�̕`��
	void DrawMenu()const;
	//�Z�[�u���[�h�̕`��
	void DrawSave()const;
	//�i�q�̕`��
	void DrawFrame()const;

	//�}�b�v�`�b�v���쐬����
	void MakeMapChip();

	//CSV�t�@�C���ւ̏����o��(�V�K�ǉ�)
	void SaveStage(int stage_num);
	//�㏑���ۑ�
	void SaveStage(char* stage_name);
};
