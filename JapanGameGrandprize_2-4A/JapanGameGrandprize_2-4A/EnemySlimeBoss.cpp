//#include<dxlib.h>
//#define _USE_MATH_DEFINES
//#include<math.h>
//#include"EnemySlimeBoss.h"
//#include"Player.h"
//#include"Stage/Stage.h"
//
//#define SLIME_ATTACK_DISTANCE_Y 15
//#define SLIME_ATTACK_SPEED 5
//
////�ړ����x
//#define SLIME_SPEED 2
//#define SLIME_ATTACK_DAMAGE 10
//
//#define SLIME_MIN_DROP 0u
//#define SLIME_MAX_DROP 3u
//
//#define GROUND 1200
//#define WAIT_TIME 30 //�v���C���[�������čU������܂ł̎���
//
//#define ONE_ROUND 360 //����̊p�x
//#define ROTATION_SPEED 15 //�X���C������]����X�s�[�h
//
//
//EnemySlimeBoss::EnemySlimeBoss()
//{
//	attack = false;
//	left_move = true;
//	kind = ENEMY_KIND::SLIME;
//
//	location.x = 1690.0f;
//	location.y = 980.0f;
//
//	area.height = 40;
//	area.width = 46;
//
//	hp = 100;
//	speed = SLIME_SPEED;
//
//	type = new ENEMY_TYPE;
//	*type = ENEMY_TYPE::WATER;
//
//	state = ENEMY_STATE::IDOL;
//	slimeboss_image = LoadGraph("Images/Enemy/Slime_.png");
//	
//	//�h���b�v�A�C�e���̐ݒ�
//	drop_element = new ElementItem * [SOIL_DROP];
//	drop_type_volume = SOIL_DROP;
//
//	int volume = 0;
//
//	for (int i = 0; i < WATER_DROP; i++)
//	{
//		volume = SLIME_MIN_DROP + GetRand(SLIME_MAX_DROP);
//		drop_element[i] = new ElementItem(static_cast<ELEMENT_ITEM>(2 + i));
//		drop_element[i]->SetVolume(volume);
//		drop_volume += volume;
//	}
//
//}
//
//void EnemySlimeBoss::Update(const Player* player, const Stage* stage)
//{
//	Location old_location = location;	//�O�̍��W
//	HitMapChip hit_stage = { false,nullptr }; //�X�e�[�W�Ƃ̓����蔻��
//
//	switch (state)
//	{
//	case ENEMY_STATE::IDOL:
//		Idol();
//		break;
//	case ENEMY_STATE::MOVE:
//		Move(player->GetLocation());
//
//		hit_stage = HitStage(stage);
//		if (hit_stage.hit) //�X�e�[�W�Ƃ̓����蔻��
//		{
//			Location chip_location = hit_stage.chip->GetLocation();
//			Area chip_area = hit_stage.chip->GetArea();
//			if ((chip_location.y + chip_area.height / 2) < (location.y + area.height / 2))
//			{
//				if (left_move)
//				{
//					location.x = chip_location.x + (chip_area.width / 2) + (area.width / 2) + 2;
//				}
//				else
//				{
//					location.x = chip_location.x - (chip_area.width / 2) - (area.width / 2) - 2;
//				}
//				left_move = !left_move;
//				speed = -speed;
//			}
//		}
//		else
//		{
//			state = ENEMY_STATE::FALL;
//			speed = 0;
//		}
//
//		break;
//	case ENEMY_STATE::FALL:
//		Fall();
//		hit_stage = HitStage(stage);
//
//		if (hit_stage.hit) //�X�e�[�W�Ƃ̓����蔻��
//		{
//			Location chip_location = hit_stage.chip->GetLocation();
//			Area chip_area = hit_stage.chip->GetArea();
//			if ((chip_location.y - chip_area.height / 2) < (location.y + area.height / 2))
//			{
//				location.y = chip_location.y - (chip_area.height / 2) - (area.height / 2) + 2;
//				state = ENEMY_STATE::MOVE;
//				if (left_move)
//				{
//					speed = -SLIME_SPEED;
//				}
//				else
//				{
//					speed = SLIME_SPEED;
//				}
//			}
//		}
//		break;
//	case ENEMY_STATE::ATTACK:
//		Attack(player->GetLocation());
//		break;
//	case ENEMY_STATE::DEATH:
//		Death();
//		break;
//	default:
//		break;
//	}
//
//
//
//
//	if (CheckHp() && state != ENEMY_STATE::DEATH)
//	{
//		state = ENEMY_STATE::DEATH;
//		jump_distance.y = 15;
//	}
//}
//
//void EnemySlimeBoss::Draw()const
//{
//
//	Location draw_location = location;
//	Location camera = CameraWork::GetCamera();
//	draw_location = draw_location - camera;
//
//	//DrawRotaGraph(draw_location.x, draw_location.y, 0.17, M_PI / 180 * slime_angle, slime_image, TRUE, !left_move);
//}
//
////-----------------------------------
////�A�C�h�����
////-----------------------------------
//void EnemySlimeBoss::Idol()
//{
//	Location scroll; //��ʃX�N���[�����l������X���W
//	Location camera = CameraWork::GetCamera(); //�J����
//	scroll = location - camera;
//
//	if ((-area.width < scroll.x) && (scroll.x < SCREEN_WIDTH + area.width) &&
//		(-area.height < scroll.y) && (scroll.y < SCREEN_HEIGHT + area.height))
//	{
//		state = ENEMY_STATE::MOVE;
//	}
//}
//
////-----------------------------------
////�ړ�
////-----------------------------------
//void EnemySlimeBoss::Move(const Location player_location)
//{
//
//	float distance; //����Ă��鋗��
//
//	//�v���C���[�Ƃ̋����̌v�Z
//	distance = sqrtf(powf(player_location.x - location.x, 2) + powf(player_location.y - location.y, 2));
//
//}
//
////-----------------------------------
////����
////-----------------------------------
//void EnemySlimeBoss::Fall()
//{
//	if (speed < GRAVITY)
//	{
//		speed += ENEMY_FALL_SPEED;
//	}
//	location.y += speed;
//}
//
////-----------------------------------
////�U��
////-----------------------------------
//void  EnemySlimeBoss::Attack(Location player_location)
//{
//	location.y -= (jump_distance.y / 3);
//	jump_distance.y -= 1;
//
//	switch (slime_attack)
//	{
//	case SLIME_ATTACK::BEFORE_ATTACK:
//
//		if (left_move)
//		{
//			speed = -SLIME_ATTACK_SPEED;
//		}
//		else
//		{
//			speed = SLIME_ATTACK_SPEED;
//		}
//		break;
//
//	case SLIME_ATTACK::AFTER_ATTACK:
//
//		if (left_move)
//		{
//			speed = -SLIME_ATTACK_SPEED;
//		}
//		else
//		{
//			speed = SLIME_ATTACK_SPEED;
//		}
//
//		break;
//	}
//	location.x += speed;
//
//	if (location.y >= GROUND)
//	{
//		attack = false;
//		slime_attack = SLIME_ATTACK::BEFORE_ATTACK;
//		state = ENEMY_STATE::MOVE;
//		if (left_move)
//		{
//			speed = -SLIME_SPEED;
//		}
//		else
//		{
//			speed = SLIME_SPEED;
//
//		}
//	}
//}
//
////-----------------------------------
////�U�����������Ă��邩
////-----------------------------------
//AttackResource EnemySlimeBoss::Hit()
//{
//	AttackResource ret = { 0,nullptr,0 }; //�߂�l
//
//	if (!attack)
//	{
//		attack = true;
//		slime_attack = SLIME_ATTACK::AFTER_ATTACK;
//		ENEMY_TYPE attack_type[1] = { *type };
//		ret.damage = SLIME_ATTACK_DAMAGE;
//		ret.type = attack_type;
//		ret.type_count = 1;
//	}
//
//	return ret;
//}
//
////-----------------------------------
////���S
////-----------------------------------
//void EnemySlimeBoss::Death()
//{
//	if (slime_angle >= (ONE_ROUND * 2.5) || slime_angle <= -(ONE_ROUND * 2.5))
//	{
//		can_delete = true;
//	}
//	else
//	{
//		if (location.y <= GROUND)
//		{
//			location.y -= (jump_distance.y / 3);
//			jump_distance.y--;
//		}
//		if (left_move)
//		{
//			speed = SLIME_ATTACK_SPEED;
//			slime_angle += ROTATION_SPEED;
//		}
//		else
//		{
//
//			speed = -SLIME_ATTACK_SPEED;
//			slime_angle -= ROTATION_SPEED;
//		}
//		location.x += speed;
//	}
//}
//
////-----------------------------------
//// �v���C���[�̒e�Ƃ̓����蔻��
////-----------------------------------
//void EnemySlimeBoss::HitBullet(const BulletBase* bullet)
//{
//	switch (bullet->GetAttribute())
//	{
//	case ATTRIBUTE::NORMAL:
//		hp -= bullet->GetDamage() * RESISTANCE_DAMAGE;
//		break;
//	case ATTRIBUTE::EXPLOSION:
//		hp -= bullet->GetDamage() * WEAKNESS_DAMAGE;
//		break;
//	case ATTRIBUTE::MELT:
//		hp -= bullet->GetDamage() * WEAKNESS_DAMAGE;
//		break;
//	case ATTRIBUTE::POISON:
//		//poison_damage = bullet->GetDamage();
//		//poison_time = bullet->GetDebuffTime() * RESISTANCE_DEBUFF;
//		break;
//	case ATTRIBUTE::PARALYSIS:
//		paralysis_time = bullet->GetDebuffTime() * 0;
//		break;
//	case ATTRIBUTE::HEAL:
//		break;
//	default:
//		break;
//	}
//
//}
//
////-----------------------------------
////���W�̎擾
////-----------------------------------
//Location EnemySlimeBoss::GetLocation() const
//{
//	return location;
//}