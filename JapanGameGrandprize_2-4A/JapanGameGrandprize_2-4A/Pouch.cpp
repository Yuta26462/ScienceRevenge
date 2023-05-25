#include "Pouch.h"
#include "DxLib.h"
#include "PadInput.h"
#include "Define.h"
#include "EnumElementItem.h"
#include <stdlib.h>
#include <math.h>

#define STRING_DISTANCE 100

#define CURSOL_MOVE 48
#define EXPLOSION_NUM 0
#define MELT_NUM 1
#define POISON_NUM 2
#define PARARYSIS_NUM 3
#define HEAL_NUM 4

#define OPTION_ZERO OPTION_ONE - STRING_DISTANCE
#define OPTION_ONE 140
#define OPTION_TWO OPTION_ONE + STRING_DISTANCE
#define OPTION_THREE OPTION_TWO + STRING_DISTANCE
#define OPTION_FOUR OPTION_THREE + STRING_DISTANCE

#ifdef DEBUG_PV

#endif // DEBUG_PV



//ÉRÉìÉXÉgÉâÉNÉ^
Pouch::Pouch()
{
	old = PAD_INPUT::GetRStick().x;
	x = 1040;
	y = 100;
	window_x = x - 140;
	window_y = y - 15;
	attribute_name_x = x - 45;
	attribute_name_y = y + 40;
	cursol = 0;
	for (int i = 0; i < 5; i++)
	{
		each_cursor[i] = 0;
		each_page[i] = 0;
	}
	count = 0;
	page = 0;
	move_string = 0;
	page_jump_count = 0;
	on_bool = false;
	move_up = false;
	move_down = false;
	tab = ATTRIBUTE::EXPLOSION;

	//âÊëú
	second_tab_image[0] = LoadGraph("Images/ItemTab/explosion/melt_explosion.png");
	second_tab_image[1] = LoadGraph("Images/ItemTab/explosion/poison.png");
	second_tab_image[2] = LoadGraph("Images/ItemTab/explosion/paralysis.png");
	second_tab_image[3] = LoadGraph("Images/ItemTab/explosion/heal.png");
	elemental_count = LoadGraph("Images/ItemTab/P_kazu.png");
	window_image = LoadGraph("Images/ItemTab/poti_back.png");
	window_image_two = LoadGraph("Images/ItemTab/poti_waku.png");
	stick_image_l = LoadGraph("Images/ItemTab/Rstick_L.png");
	stick_image_r = LoadGraph("Images/ItemTab/Rstick_R.png");
	text_window_image_lit = LoadGraph("Images/ItemTab/P_Text_BackBox.png");
	text_window_image_dark = LoadGraph("Images/ItemTab/P_Text_nBackBox.png");
	cursor_image = LoadGraph("Images/ItemTab/Cursor.png");
	attribute_images = new int[ATTRIBUTE_IMAGES];
	attribute_text_images = new int[ATTRIBUTE_IMAGES];
	LoadDivGraph("Images/ItemTab/Poach_Name.png", 5, 1, 5, 212, 45, attribute_text_images);
	LoadDivGraph("Images/Player/zokusei_icon_x2.png", 10, 5, 2, 55, 51, attribute_images);
	int images_init_explosion[EXPLOSION_MAX_NUM];
	int images_init_melt[MELT_MAX_NUM];
	int images_init_pararysis[PARARYSIS_MAX_NUM];
	int images_init_poison[POISON_MAX_NUM];
	int images_init_heal[HEAL_MAX_NUM];

	//îöî≠ÇÃâªçáï®ÇÃñºëOâÊëú
	LoadDivGraph("Images/ItemTab/Secret_FILE/explosion.png", EXPLOSION_MAX_NUM, 1, EXPLOSION_MAX_NUM, 350, 23, images_init_explosion);
	for (int i = 0; i < EXPLOSION_MAX_NUM; i++)
	{
		chemical_formula_explosion[i].name_image = images_init_explosion[i];
	}
	LoadDivGraph("Images/ItemTab/Secret_Dark_FILE/explosion.png", EXPLOSION_MAX_NUM, 1, EXPLOSION_MAX_NUM, 350, 23, images_init_explosion);
	for (int i = 0; i < EXPLOSION_MAX_NUM; i++)
	{
		chemical_formula_explosion[i].dark_name_image = images_init_explosion[i];
	}
	LoadDivGraph("Images/ItemTab/HP_Text/HP_explosion.png", EXPLOSION_MAX_NUM, 1, EXPLOSION_MAX_NUM, 350, 23, images_init_explosion);
	for (int i = 0; i < EXPLOSION_MAX_NUM; i++)
	{
		chemical_formula_explosion[i].ui_name_image = images_init_explosion[i];
	}

	//ónâÇÃâªçáï®ÇÃñºëOâÊëú
	LoadDivGraph("Images/ItemTab/Secret_FILE/melt.png", MELT_MAX_NUM, 1, MELT_MAX_NUM, 350, 23, images_init_melt);
	for (int i = 0; i < MELT_MAX_NUM; i++)
	{
		chemical_formula_melt[i].name_image = images_init_melt[i];
	}
	LoadDivGraph("Images/ItemTab/Secret_Dark_FILE/melt.png", MELT_MAX_NUM, 1, MELT_MAX_NUM, 350, 23, images_init_melt);
	for (int i = 0; i < MELT_MAX_NUM; i++)
	{
		chemical_formula_melt[i].dark_name_image = images_init_melt[i];
	}
	LoadDivGraph("Images/ItemTab/HP_Text/HP_melt.png", MELT_MAX_NUM, 1, MELT_MAX_NUM, 350, 23, images_init_melt);
	for (int i = 0; i < MELT_MAX_NUM; i++)
	{
		chemical_formula_melt[i].ui_name_image = images_init_melt[i];
	}

	//ñÉ·ÉÇÃâªçáï®ÇÃñºëOÇ™ÇºÇ§
	LoadDivGraph("Images/ItemTab/Secret_FILE/paralysis.png", PARARYSIS_MAX_NUM, 1, PARARYSIS_MAX_NUM, 350, 23, images_init_pararysis);
	for (int i = 0; i < PARARYSIS_MAX_NUM; i++)
	{
		chemical_formula_pararysis[i].name_image = images_init_pararysis[i];
	}
	LoadDivGraph("Images/ItemTab/Secret_Dark_FILE/paralysis.png", PARARYSIS_MAX_NUM, 1, PARARYSIS_MAX_NUM, 350, 23, images_init_pararysis);
	for (int i = 0; i < PARARYSIS_MAX_NUM; i++)
	{
		chemical_formula_pararysis[i].dark_name_image = images_init_pararysis[i];
	}
	LoadDivGraph("Images/ItemTab/HP_Text/HP_paralysis.png", PARARYSIS_MAX_NUM, 1, PARARYSIS_MAX_NUM, 350, 23, images_init_pararysis);
	for (int i = 0; i < PARARYSIS_MAX_NUM; i++)
	{
		chemical_formula_pararysis[i].ui_name_image = images_init_pararysis[i];
	}

	//ì≈ÇÃâªçáï®ÇÃñºëOâÊëú
	LoadDivGraph("Images/ItemTab/Secret_FILE/poison.png", POISON_MAX_NUM, 1, POISON_MAX_NUM, 350, 23, images_init_poison);
	for (int i = 0; i < POISON_MAX_NUM; i++)
	{
		chemical_formula_poison[i].name_image = images_init_poison[i];
	}
	LoadDivGraph("Images/ItemTab/Secret_Dark_FILE/poison.png", POISON_MAX_NUM, 1, POISON_MAX_NUM, 350, 23, images_init_poison);
	for (int i = 0; i < POISON_MAX_NUM; i++)
	{
		chemical_formula_poison[i].dark_name_image = images_init_poison[i];
	}
	LoadDivGraph("Images/ItemTab/HP_Text/HP_poison.png", POISON_MAX_NUM, 1, POISON_MAX_NUM, 350, 23, images_init_poison);
	for (int i = 0; i < POISON_MAX_NUM; i++)
	{
		chemical_formula_poison[i].ui_name_image = images_init_poison[i];
	}

	//âÒïúÇÃâªçáï®ÇÃñºëOâÊëú
	LoadDivGraph("Images/ItemTab/Secret_FILE/heal.png", HEAL_MAX_NUM, 1, HEAL_MAX_NUM, 350, 23, images_init_heal);
	for (int i = 0; i < HEAL_MAX_NUM; i++)
	{
		chemical_formula_heal[i].name_image = images_init_heal[i];
	}
	LoadDivGraph("Images/ItemTab/Secret_Dark_FILE/heal.png", HEAL_MAX_NUM, 1, HEAL_MAX_NUM, 350, 23, images_init_heal);
	for (int i = 0; i < HEAL_MAX_NUM; i++)
	{
		chemical_formula_heal[i].dark_name_image = images_init_heal[i];
	}
	LoadDivGraph("Images/ItemTab/HP_Text/HP_heal.png", HEAL_MAX_NUM, 1, HEAL_MAX_NUM, 350, 23, images_init_heal);
	for (int i = 0; i < HEAL_MAX_NUM; i++)
	{
		chemical_formula_heal[i].ui_name_image = images_init_heal[i];
	}

	craft = LoadSoundMem("Sounds/SE/Stage/PlayerCraft/craft.wav");
	cancel = LoadSoundMem("Sounds/SE/Stage/PlayerCraft/cancel.wav");
	cursor_move = LoadSoundMem("Sounds/SE/Stage/PlayerCraft/menumove.wav");

	//å≥ëfÇÃèâä˙âª
	for (int i = 0; i < PLAYER_ELEMENT; i++)
	{
		element[i] = nullptr;
	}

	InitChemicalParameter();

	InitializeExplosion();

	InitializeMelt();

	InitializePararysis();

	InitializeHeal();

	InitializePoison();

	element_char[0] = 'H';
	element_char[1] = 'O';
	element_char[2] = 'C';
	element_char[3] = 'N';
	element_char[4] = 'S';
	element_char[5] = 'Ch';
	element_char[6] = 'U';
}

Pouch::~Pouch()
{

}


//îöî≠
void Pouch::ExplosionTabDraw() const
{
	DrawGraph(x, y + 300, second_tab_image[0], TRUE);

#ifdef DEBUG_PV
	DrawFormatString(x - 560, y, 0xffffff, "%s", chemical_formula_explosion[cursol].chemical_formula);
#endif // DEBUG_PV

	DrawFormatString(x + 165, y + 380, 0xffffff, "%d", chemical_formula_explosion[cursol].number_of_bullets);
	DrawFormatString(x + 165, y + 410, 0xffffff, "%d", chemical_formula_explosion[cursol].damage);



	for (int i = 0; i < EXPLOSION_MAX_NUM; i++)
	{
		if (ComparisonElement(chemical_formula_explosion[i]))
		{
			if (select_explosion.make_bool)
			{
				if (select_explosion.tag_number == chemical_formula_explosion[i].tag_number)
				{
					DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_lit, true);
					DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_explosion[i].name_image, true);
				}
				else
				{
					DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_dark, true);
					DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_explosion[i].dark_name_image, true);
				}
			}
			else
			{
				DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_lit, true);
				DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_explosion[i].name_image, true);
			}
		}
		else
		{
			DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_dark, true);
			DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_explosion[i].dark_name_image, true);
		}
	}


	ElementDraw(chemical_formula_explosion[cursol]);

	DrawGraph(attribute_name_x - 40, attribute_name_y - 5, attribute_text_images[0], true);
}


//ónâ
void Pouch::MeltTabDraw() const
{
	DrawGraph(x, y + 300, second_tab_image[0], TRUE);


#ifdef DEBUG_PV
	DrawFormatString(x - 560, y, 0xffffff, "%s", chemical_formula_melt[cursol].chemical_formula);
#endif // DEBUG_PV
	
	DrawFormatString(x + 165, y + 380, 0xffffff, "%d", chemical_formula_melt[cursol].number_of_bullets);
	DrawFormatString(x + 165, y + 410, 0xffffff, "%d", chemical_formula_melt[cursol].damage);


	for (int i = 0; i < MELT_MAX_NUM; i++)
	{
		if (ComparisonElement(chemical_formula_melt[i]))
		{
			if (select_melt.make_bool)
			{
				if (select_melt.tag_number == chemical_formula_melt[i].tag_number)
				{
					DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_lit, true);
					DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_melt[i].name_image, true);
				}
				else
				{
					DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_dark, true);
					DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_melt[i].dark_name_image, true);
				}
			}
			else
			{
				DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_lit, true);
				DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_melt[i].name_image, true);
			}
		}
		else
		{
			DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_dark, true);
			DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_melt[i].dark_name_image, true);
		}
	}



	ElementDraw(chemical_formula_melt[cursol]);


	DrawGraph(attribute_name_x, attribute_name_y - 5, attribute_text_images[1], true);
}

//ñÉ·É
void Pouch::ParalysisTabDraw()const
{
	DrawGraph(x, y + 300, second_tab_image[2], TRUE);


#ifdef DEBUG_PV
	DrawFormatString(x - 560, y, 0xffffff, "%s", chemical_formula_pararysis[cursol].chemical_formula);
#endif // DEBUG_PV
	
	DrawFormatString(x + 165, y + 380, 0xffffff, "%d", chemical_formula_pararysis[cursol].number_of_bullets);
	DrawFormatString(x + 165, y + 410, 0xffffff, "%d", chemical_formula_pararysis[cursol].damage);
	DrawFormatString(x + 165, y + 440, 0xffffff, "%d", chemical_formula_pararysis[cursol].time / 60);
	DrawString(x + 190, y + 440, "ïb", 0xffffff);

	for (int i = 0; i < PARARYSIS_MAX_NUM; i++)
	{
		if (ComparisonElement(chemical_formula_pararysis[i]))
		{
			if (select_pararysis.make_bool)
			{
				if (select_pararysis.tag_number == chemical_formula_pararysis[i].tag_number)
				{
					DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_lit, true);
					DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_pararysis[i].name_image, true);
				}
				else
				{
					DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_dark, true);
					DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_pararysis[i].dark_name_image, true);
				}
			}
			else
			{
				DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_lit, true);
				DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_pararysis[i].name_image, true);
			}
		}
		else
		{
			DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_dark, true);
			DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_pararysis[i].dark_name_image, true);
		}
	}



	ElementDraw(chemical_formula_pararysis[cursol]);



	DrawGraph(attribute_name_x - 30, attribute_name_y - 5, attribute_text_images[3], true);
}

//ì≈
void Pouch::PoisonTabDraw()const
{
	DrawGraph(x, y + 300, second_tab_image[1], TRUE);

#ifdef DEBUG_PV
	DrawFormatString(x - 560, y, 0xffffff, "%s", chemical_formula_poison[cursol].chemical_formula);
#endif // DEBUG_PV
	
	DrawFormatString(x + 165, y + 380, 0xffffff, "%d", chemical_formula_poison[cursol].number_of_bullets);
	DrawFormatString(x + 165, y + 410, 0xffffff, "%d", chemical_formula_poison[cursol].damage);
	DrawFormatString(x + 165, y + 440, 0xffffff, "%d", chemical_formula_poison[cursol].damage_per_second);
	DrawFormatString(x + 165, y + 470, 0xffffff, "%d", chemical_formula_poison[cursol].time / 60);
	DrawString(x + 190, y + 470, "ïb", 0xffffff);



	for (int i = 0; i < POISON_MAX_NUM; i++)
	{
		if (ComparisonElement(chemical_formula_poison[i]))
		{
			if (select_poison.make_bool)
			{
				if (select_poison.tag_number == chemical_formula_poison[i].tag_number)
				{
					DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_lit, true);
					DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_poison[i].name_image, true);
				}
				else
				{
					DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_dark, true);
					DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_poison[i].dark_name_image, true);
				}
			}
			else
			{
				DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_lit, true);
				DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_poison[i].name_image, true);
			}
		}
		else
		{
			DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_dark, true);
			DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_poison[i].dark_name_image, true);
		}
	}

	ElementDraw(chemical_formula_poison[cursol]);


	DrawGraph(attribute_name_x, attribute_name_y - 5, attribute_text_images[2], true);
}
//âÒïú
void Pouch::HealTabDraw()const
{
	DrawGraph(x, y + 300, second_tab_image[3], TRUE);

#ifdef DEBUG_PV
	DrawFormatString(x - 560, y, 0xffffff, "%s", chemical_formula_heal[cursol].chemical_formula);
#endif // DEBUG_PV
	
	DrawFormatString(x + 165, y + 380, 0xffffff, "%d", chemical_formula_heal[cursol].number_of_bullets);
	DrawFormatString(x + 165, y + 410, 0xffffff, "%d", chemical_formula_heal[cursol].damage);



	for (int i = 0; i < HEAL_MAX_NUM; i++)
	{
		if (ComparisonElement(chemical_formula_heal[i]))
		{
			if (select_heal.make_bool)
			{
				if (select_heal.tag_number == chemical_formula_heal[i].tag_number)
				{
					DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_lit, true);
					DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_heal[i].name_image, true);
				}
				else
				{
					DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_dark, true);
					DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_heal[i].dark_name_image, true);
				}
			}
			else
			{
				DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_lit, true);
				DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_heal[i].name_image, true);
			}
		}
		else
		{
			DrawGraph(x - 140, y + 80 + (CURSOL_MOVE * i), text_window_image_dark, true);
			DrawGraph(x - 130, y + 90 + (CURSOL_MOVE * i), chemical_formula_heal[i].dark_name_image, true);
		}
	}


	ElementDraw(chemical_formula_heal[cursol]);

	DrawGraph(attribute_name_x, attribute_name_y - 5, attribute_text_images[4], true);
}

//ï`âÊ
void Pouch::Draw() const
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 127);
	DrawGraph(x - 210, y - 50, window_image, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);

	DrawGraph(x - 197, y - 55, window_image_two, TRUE);

	DrawGraph(window_x, window_y, attribute_images[0], TRUE);
	DrawGraph(window_x + (65 * 1), window_y, attribute_images[4], TRUE);
	DrawGraph(window_x + (65 * 2), window_y, attribute_images[1], TRUE);
	DrawGraph(window_x + (65 * 3), window_y, attribute_images[2], TRUE);
	DrawGraph(window_x + (65 * 4), window_y, attribute_images[3], TRUE);


	switch (tab)
	{
	case ATTRIBUTE::EXPLOSION:
		ExplosionTabDraw();
		DrawGraph(window_x, window_y, attribute_images[5], TRUE);
		break;
	case ATTRIBUTE::MELT:
		MeltTabDraw();
		DrawGraph(window_x + (65 * 1), window_y, attribute_images[9], TRUE);
		break;
	case ATTRIBUTE::POISON:
		PoisonTabDraw();
		DrawGraph(window_x + (65 * 2), window_y, attribute_images[6], TRUE);
		break;
	case ATTRIBUTE::PARALYSIS:
		ParalysisTabDraw();
		DrawGraph(window_x + (65 * 3), window_y, attribute_images[7], TRUE);
		break;
	case ATTRIBUTE::HEAL:
		HealTabDraw();
		DrawGraph(window_x + (65 * 4), window_y, attribute_images[8], TRUE);
		break;
	default:
		break;
	}
	DrawGraph(x - 150, y + 80 + move_string, cursor_image, true);
	DrawGraph(x - 150, y + 80 + move_string, cursor_image, true);
	DrawGraph(x - 170, y + 360, elemental_count, TRUE);
	DrawRotaGraph(x - 120, attribute_name_y + 15,0.9,0, stick_image_l, true);
	DrawRotaGraph(x + 180, attribute_name_y + 15, 0.9, 0, stick_image_r, true);
}

void Pouch::ElementDraw(ChemicalFormulaParameter bring) const
{
	int i = 0;
	int tab_x = x - 100;

	//CÇÃï`âÊ
	DrawFormatString(tab_x, y + 382, 0xffffff, "%3d",
		element[static_cast<int>(ELEMENT_ITEM::CARBON)]->GetVolume());
	int c = element[static_cast<int>(ELEMENT_ITEM::CARBON)]->GetVolume() - bring.material.carbon;
	if (c < 0)
	{
		DrawFormatString(x - 20, y + 382, 0xff0000, "%3d",c);
	}
	else
	{
		DrawFormatString(x - 20, y + 382, 0xffffff, "%3d",c);
	}

	//HÇÃï`âÊ
	DrawFormatString(tab_x, y + 415 + (30 * i), 0xffffff, "%3d",
		element[static_cast<int>(ELEMENT_ITEM::HYDROGEN)]->GetVolume());
	int h = element[static_cast<int>(ELEMENT_ITEM::HYDROGEN)]->GetVolume() - bring.material.hydrogen;
	if (h < 0)
	{
		DrawFormatString(x - 20, y + 415 + (30 * i), 0xff0000, "%3d",h);
	}
	else
	{
		DrawFormatString(x - 20, y + 415 + (30 * i), 0xffffff, "%3d", h);
	}

	//NÇÃï`âÊ
	DrawFormatString(tab_x, y + 445 + (30 * i), 0xffffff, "%3d",
		element[static_cast<int>(ELEMENT_ITEM::NITROGEN)]->GetVolume());
	int n = element[static_cast<int>(ELEMENT_ITEM::NITROGEN)]->GetVolume() - bring.material.nitrogen;
	if (n < 0)
	{
		DrawFormatString(x - 20, y + 445 + (30 * i), 0xff0000, "%3d",n);
	}
	else
	{
		DrawFormatString(x - 20, y + 445 + (30 * i), 0xffffff, "%3d",n);
	}

	//OÇÃï`âÊ
	DrawFormatString(tab_x, y + 475 + (30 * i), 0xffffff, "%3d",
		element[static_cast<int>(ELEMENT_ITEM::OXYGEN)]->GetVolume());
	int o = element[static_cast<int>(ELEMENT_ITEM::OXYGEN)]->GetVolume() - bring.material.oxygen;
	if (o < 0)
	{
		DrawFormatString(x - 20, y + 475 + (30 * i), 0xff0000, "%3d",o);
	}
	else
	{
		DrawFormatString(x - 20, y + 475 + (30 * i), 0xffffff, "%3d",o);
	}

	//SÇÃï`âÊ
	DrawFormatString(tab_x, y + 505 + (30 * i), 0xffffff, "%3d",
		element[static_cast<int>(ELEMENT_ITEM::SULFUR)]->GetVolume());
	int s = element[static_cast<int>(ELEMENT_ITEM::SULFUR)]->GetVolume() - bring.material.sulfur;
	if (s < 0)
	{
		DrawFormatString(x - 20, y + 505 + (30 * i), 0xff0000, "%3d",s);
	}
	else
	{
		DrawFormatString(x - 20, y + 505 + (30 * i), 0xffffff, "%3d",s);
	}

	//CHÇÃï`âÊ
	DrawFormatString(tab_x, y + 535 + (30 * i), 0xffffff, "%3d",
		element[static_cast<int>(ELEMENT_ITEM::CHLORINE)]->GetVolume());
	int ch = element[static_cast<int>(ELEMENT_ITEM::CHLORINE)]->GetVolume() - bring.material.chlorine;
	if (ch < 0)
	{
		DrawFormatString(x - 20, y + 535 + (30 * i), 0xff0000, "%3d", ch);
	}
	else
	{
		DrawFormatString(x - 20, y + 535 + (30 * i), 0xffffff, "%3d", ch);
	}

}

//ÉAÉbÉvÉfÅ[Ég
void Pouch::Update()
{
	if (old < 15000)
	{
		if (15000 < PAD_INPUT::GetRStick().x)
		{
			move_down = false;
			move_up = false;
			PlaySoundMem(cursor_move, DX_PLAYTYPE_BACK);
			switch (tab)
			{
			case ATTRIBUTE::EXPLOSION:
				each_cursor[EXPLOSION_NUM] = cursol;
				cursol = each_cursor[MELT_NUM];
				move_string = (CURSOL_MOVE * cursol);
				tab = ATTRIBUTE::MELT;
				break;
			case ATTRIBUTE::MELT:
				each_cursor[MELT_NUM] = cursol;
				cursol = each_cursor[POISON_NUM];
				move_string = (CURSOL_MOVE * cursol);
				tab = ATTRIBUTE::POISON;
				break;
			case ATTRIBUTE::POISON:
				each_cursor[POISON_NUM] = cursol;
				cursol = each_cursor[PARARYSIS_NUM];
				move_string = (CURSOL_MOVE * cursol);
				tab = ATTRIBUTE::PARALYSIS;
				break;
			case ATTRIBUTE::PARALYSIS:
				each_cursor[PARARYSIS_NUM] = cursol;
				cursol = each_cursor[HEAL_NUM];
				move_string = (CURSOL_MOVE * cursol);
				tab = ATTRIBUTE::HEAL;
				break;
			case ATTRIBUTE::HEAL:
				each_cursor[HEAL_NUM] = cursol;
				cursol = each_cursor[EXPLOSION_NUM];
				move_string = (CURSOL_MOVE * cursol);
				tab = ATTRIBUTE::EXPLOSION;
				break;
			default:
				break;
			}
		}

	}

	if (old > -15000)
	{
		if (-15000 > PAD_INPUT::GetRStick().x)
		{
			move_down = false;
			move_up = false;
			PlaySoundMem(cursor_move, DX_PLAYTYPE_BACK);
			switch (tab)
			{
			case ATTRIBUTE::EXPLOSION:
				each_cursor[EXPLOSION_NUM] = cursol;
				cursol = each_cursor[HEAL_NUM];
				move_string = (CURSOL_MOVE * cursol);
				tab = ATTRIBUTE::HEAL;
				break;
			case ATTRIBUTE::MELT:
				each_cursor[MELT_NUM] = cursol;
				cursol = each_cursor[EXPLOSION_NUM];
				move_string = (CURSOL_MOVE * cursol);
				tab = ATTRIBUTE::EXPLOSION;
				break;
			case ATTRIBUTE::POISON:
				each_cursor[POISON_NUM] = cursol;
				cursol = each_cursor[MELT_NUM];
				move_string = (CURSOL_MOVE * cursol);
				tab = ATTRIBUTE::MELT;
				break;
			case ATTRIBUTE::PARALYSIS:
				each_cursor[PARARYSIS_NUM] = cursol;
				cursol = each_cursor[POISON_NUM];
				move_string = (CURSOL_MOVE * cursol);
				tab = ATTRIBUTE::POISON;
				break;
			case ATTRIBUTE::HEAL:
				each_cursor[HEAL_NUM] = cursol;
				cursol = each_cursor[PARARYSIS_NUM];
				move_string = (CURSOL_MOVE * cursol);
				tab = ATTRIBUTE::PARALYSIS;
				break;
			default:
				break;
			}
		}
	}

	switch (tab)
	{
	case ATTRIBUTE::EXPLOSION:
		TabUpdate(EXPLOSION_MAX_NUM);
		break;
	case ATTRIBUTE::MELT:
		TabUpdate(MELT_MAX_NUM);
		break;
	case ATTRIBUTE::POISON:
		TabUpdate(POISON_MAX_NUM);
		break;
	case ATTRIBUTE::PARALYSIS:
		TabUpdate(PARARYSIS_MAX_NUM);
		break;
	case ATTRIBUTE::HEAL:
		TabUpdate(HEAL_MAX_NUM);
		break;
	default:
		break;
	}

	if (PAD_INPUT::OnButton(XINPUT_BUTTON_A))
	{
		on_bool = true;
		switch (tab)
		{
		case ATTRIBUTE::EXPLOSION:
			if (!select_explosion.make_bool || 
				select_explosion.tag_number == chemical_formula_explosion[cursol].tag_number)
			{

				if (ComparisonElement(chemical_formula_explosion[cursol]))
				{
					PlaySoundMem(craft, DX_PLAYTYPE_BACK);
					if (select_explosion.tag_number == chemical_formula_explosion[cursol].tag_number)
					{
						if (select_explosion.number_of_bullets < 100)
						{
							select_explosion.number_of_bullets += chemical_formula_explosion[cursol].number_of_bullets;
							ConsumptionMaterial();
						}

						if (select_explosion.number_of_bullets >= 100)
						{
							select_explosion.number_of_bullets = 100;
						}
					}
					else
					{
						SetChemicalFormulaParameter();
						ConsumptionMaterial();
					}
				}
				else
				{
					PlaySoundMem(cancel, DX_PLAYTYPE_BACK);
				}
			}
			else
			{
				PlaySoundMem(cancel, DX_PLAYTYPE_BACK);
			}
			break;
		case ATTRIBUTE::MELT:
			if (!select_melt.make_bool ||
				select_melt.tag_number == chemical_formula_melt[cursol].tag_number)
			{

				if (ComparisonElement(chemical_formula_melt[cursol]))
				{
					PlaySoundMem(craft, DX_PLAYTYPE_BACK);
					if (select_melt.tag_number == chemical_formula_melt[cursol].tag_number)
					{
						if (select_melt.number_of_bullets < 100)
						{
							select_melt.number_of_bullets += chemical_formula_melt[cursol].number_of_bullets;
							ConsumptionMaterial();
						}

						if (select_melt.number_of_bullets >= 100)
						{
							select_melt.number_of_bullets = 100;
						}
					}
					else
					{
						SetChemicalFormulaParameter();
						ConsumptionMaterial();
					}
				}
				else
				{
					PlaySoundMem(cancel, DX_PLAYTYPE_BACK);
				}
			}
			else
			{
				PlaySoundMem(cancel, DX_PLAYTYPE_BACK);
			}
			break;
		case ATTRIBUTE::POISON:
			if (!select_poison.make_bool ||
				select_poison.tag_number == chemical_formula_poison[cursol].tag_number)
			{

				if (ComparisonElement(chemical_formula_poison[cursol]))
				{
					PlaySoundMem(craft, DX_PLAYTYPE_BACK);
					if (select_poison.tag_number == chemical_formula_poison[cursol].tag_number)
					{
						if (select_poison.number_of_bullets < 100)
						{
							select_poison.number_of_bullets += chemical_formula_poison[cursol].number_of_bullets;
							ConsumptionMaterial();
						}

						if (select_poison.number_of_bullets >= 100)
						{
							select_poison.number_of_bullets = 100;
						}
					}
					else
					{
						SetChemicalFormulaParameter();
						ConsumptionMaterial();
					}
				}
				else
				{
					PlaySoundMem(cancel, DX_PLAYTYPE_BACK);
				}
			}
			else
			{
				PlaySoundMem(cancel, DX_PLAYTYPE_BACK);
			}
			break;
		case ATTRIBUTE::PARALYSIS:
			if (!select_pararysis.make_bool ||
				select_pararysis.tag_number == chemical_formula_pararysis[cursol].tag_number)
			{

				if (ComparisonElement(chemical_formula_pararysis[cursol]))
				{
					PlaySoundMem(craft, DX_PLAYTYPE_BACK);
					if (select_pararysis.tag_number == chemical_formula_pararysis[cursol].tag_number)
					{
						if (select_pararysis.number_of_bullets < 100)
						{
							select_pararysis.number_of_bullets += chemical_formula_pararysis[cursol].number_of_bullets;
							ConsumptionMaterial();
						}

						if (select_pararysis.number_of_bullets >= 100)
						{
							select_pararysis.number_of_bullets = 100;
						}
					}
					else
					{
						SetChemicalFormulaParameter();
						ConsumptionMaterial();
					}
				}
				else
				{
					PlaySoundMem(cancel, DX_PLAYTYPE_BACK);
				}
			}
			else
			{
				PlaySoundMem(cancel, DX_PLAYTYPE_BACK);
			}
			break;
		case ATTRIBUTE::HEAL:
			if (!select_heal.make_bool ||
				select_heal.tag_number == chemical_formula_heal[cursol].tag_number)
			{
				PlaySoundMem(craft, DX_PLAYTYPE_BACK);
				if (ComparisonElement(chemical_formula_heal[cursol]))
				{
					if (select_heal.tag_number == chemical_formula_heal[cursol].tag_number)
					{
						if (select_heal.number_of_bullets < 100)
						{
							select_heal.number_of_bullets += chemical_formula_heal[cursol].number_of_bullets;
							ConsumptionMaterial();
						}

						if (select_heal.number_of_bullets >= 100)
						{
							select_heal.number_of_bullets = 100;
						}
					}
					else
					{
						SetChemicalFormulaParameter();
						ConsumptionMaterial();
					}
				}
				else
				{
					PlaySoundMem(cancel, DX_PLAYTYPE_BACK);
				}
			}
			else
			{
				PlaySoundMem(cancel, DX_PLAYTYPE_BACK);
			}
			break;
		default:
			break;
		}
	}

	old = PAD_INPUT::GetRStick().x;
}

void Pouch::TabUpdate(int max_num)
{
	if (PAD_INPUT::GetRStick().y > 5000)
	{
		if (count++ % 10 == 0)
		{
			PlaySoundMem(craft, DX_PLAYTYPE_BACK);
			if (cursol > 0)
			{
				cursol--;
				move_string = (CURSOL_MOVE * cursol);
			}
			else
			{
				move_string = CURSOL_MOVE * (max_num - 1);
				cursol = max_num - 1;
			}
		}
	}

	if (PAD_INPUT::GetRStick().y < -5000)
	{
		if (count++ % 10 == 0)
		{
			PlaySoundMem(craft, DX_PLAYTYPE_BACK);
			cursol++;
			if (cursol < max_num)
			{
				move_string = (CURSOL_MOVE * cursol);
			}
			else
			{
				move_string = 0;
				cursol = 0;
			}
		}
	}
}

void Pouch::SetElement(ElementItem* item, int i)
{
	element[i] = item;
}

void Pouch::SetElementConstruct(int i, unsigned int element_volume)
{
	int a = 999;
	a = element_volume;

	switch (element[i]->GetType())
	{
	case ELEMENT_ITEM::HYDROGEN:
		element[i]->SetVolume(a);
		break;
	case ELEMENT_ITEM::OXYGEN:
		element[i]->SetVolume(a);
		break;
	case ELEMENT_ITEM::CARBON:
		element[i]->SetVolume(a);
		break;
	case ELEMENT_ITEM::NITROGEN:
		element[i]->SetVolume(a);
		break;
	case ELEMENT_ITEM::SULFUR:
		element[i]->SetVolume(a);
		break;
	case ELEMENT_ITEM::CHLORINE:
		element[i]->SetVolume(a);
		break;
	case ELEMENT_ITEM::URANIUM:
		element[i]->SetVolume(1);
		break;
	default:
		break;
	}

}

void Pouch::InitChemicalParameter()
{

	int count = EXPLOSION_MAX_NUM;
	FILE* fp; //FILEå^ç\ë¢ëÃ
	errno_t error;
	error = fopen_s(&fp, "Data/chemical_formula_data/ChemicalFormulaParameter.csv", "r");
	if (error != 0) //ÉtÉ@ÉCÉãÇ™äJÇØÇƒÇ»Ç¢
	{
		return;
	}
	else     //ÉtÉ@ÉCÉãÇ™äJÇØÇΩ
	{
		int attribute;
		char line[100];
		for (int i = 0; i < EXPLOSION_MAX_NUM && (fgets(line, 100, fp) != NULL); i++)
		{
			sscanf_s(line, "%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",//ì¸Ç¡ÇƒÇ¢ÇÈêîéöÇ‚ï∂éöÇ…çáÇÌÇπÇƒdÇ∆Ç©fÇ∆Ç©ïœÇ¶ÇÈ
				chemical_formula_explosion[i].chemical_formula_name,
				MAX_STRING,
				chemical_formula_explosion[i].chemical_formula,
				MAX_STRING,
				&chemical_formula_explosion[i].material.carbon,
				&chemical_formula_explosion[i].material.hydrogen,
				&chemical_formula_explosion[i].material.nitrogen,
				&chemical_formula_explosion[i].material.oxygen,
				&chemical_formula_explosion[i].material.sulfur,
				&chemical_formula_explosion[i].material.chlorine,
				&chemical_formula_explosion[i].number_of_bullets,
				&chemical_formula_explosion[i].damage,
				&chemical_formula_explosion[i].damage_per_second,
				&chemical_formula_explosion[i].time,
				&attribute,
				&chemical_formula_explosion[i].tag_number);
			chemical_formula_explosion[i].atribute = static_cast <ATTRIBUTE>(attribute);
			chemical_formula_explosion[i].make_bool = true;
		}
		for (int i = 0; i < POISON_MAX_NUM && (fgets(line, 100, fp) != NULL); i++)
		{
			sscanf_s(line, "%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",//ì¸Ç¡ÇƒÇ¢ÇÈêîéöÇ‚ï∂éöÇ…çáÇÌÇπÇƒdÇ∆Ç©fÇ∆Ç©ïœÇ¶ÇÈ
				chemical_formula_poison[i].chemical_formula_name,
				MAX_STRING,
				chemical_formula_poison[i].chemical_formula,
				MAX_STRING,
				&chemical_formula_poison[i].material.carbon,
				&chemical_formula_poison[i].material.hydrogen,
				&chemical_formula_poison[i].material.nitrogen,
				&chemical_formula_poison[i].material.oxygen,
				&chemical_formula_poison[i].material.sulfur,
				&chemical_formula_poison[i].material.chlorine,
				&chemical_formula_poison[i].number_of_bullets,
				&chemical_formula_poison[i].damage,
				&chemical_formula_poison[i].damage_per_second,
				&chemical_formula_poison[i].time,
				&attribute,
				&chemical_formula_poison[i].tag_number);
			chemical_formula_poison[i].atribute = static_cast <ATTRIBUTE>(attribute);
			chemical_formula_poison[i].make_bool = true;
		}

		for (int i = 0; i < PARARYSIS_MAX_NUM && (fgets(line, 100, fp) != NULL); i++)
		{
			sscanf_s(line, "%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",//ì¸Ç¡ÇƒÇ¢ÇÈêîéöÇ‚ï∂éöÇ…çáÇÌÇπÇƒdÇ∆Ç©fÇ∆Ç©ïœÇ¶ÇÈ
				chemical_formula_pararysis[i].chemical_formula_name,
				MAX_STRING,
				chemical_formula_pararysis[i].chemical_formula,
				MAX_STRING,
				&chemical_formula_pararysis[i].material.carbon,
				&chemical_formula_pararysis[i].material.hydrogen,
				&chemical_formula_pararysis[i].material.nitrogen,
				&chemical_formula_pararysis[i].material.oxygen,
				&chemical_formula_pararysis[i].material.sulfur,
				&chemical_formula_pararysis[i].material.chlorine,
				&chemical_formula_pararysis[i].number_of_bullets,
				&chemical_formula_pararysis[i].damage,
				&chemical_formula_pararysis[i].damage_per_second,
				&chemical_formula_pararysis[i].time,
				&attribute,
				&chemical_formula_pararysis[i].tag_number);
			chemical_formula_pararysis[i].atribute = static_cast <ATTRIBUTE>(attribute);
			chemical_formula_pararysis[i].make_bool = true;
		}

		for (int i = 0; i < HEAL_MAX_NUM && (fgets(line, 100, fp) != NULL); i++)
		{
			sscanf_s(line, "%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",//ì¸Ç¡ÇƒÇ¢ÇÈêîéöÇ‚ï∂éöÇ…çáÇÌÇπÇƒdÇ∆Ç©fÇ∆Ç©ïœÇ¶ÇÈ
				chemical_formula_heal[i].chemical_formula_name,
				MAX_STRING,
				chemical_formula_heal[i].chemical_formula,
				MAX_STRING,
				&chemical_formula_heal[i].material.carbon,
				&chemical_formula_heal[i].material.hydrogen,
				&chemical_formula_heal[i].material.nitrogen,
				&chemical_formula_heal[i].material.oxygen,
				&chemical_formula_heal[i].material.sulfur,
				&chemical_formula_heal[i].material.chlorine,
				&chemical_formula_heal[i].number_of_bullets,
				&chemical_formula_heal[i].damage,
				&chemical_formula_heal[i].damage_per_second,
				&chemical_formula_heal[i].time,
				&attribute, 
				&chemical_formula_heal[i].tag_number);
			chemical_formula_heal[i].atribute = static_cast <ATTRIBUTE>(attribute);
			chemical_formula_heal[i].make_bool = true;

		}

		for (int i = 0; i < MELT_MAX_NUM && (fgets(line, 100, fp) != NULL); i++)
		{
			sscanf_s(line, "%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",//ì¸Ç¡ÇƒÇ¢ÇÈêîéöÇ‚ï∂éöÇ…çáÇÌÇπÇƒdÇ∆Ç©fÇ∆Ç©ïœÇ¶ÇÈ
				chemical_formula_melt[i].chemical_formula_name,
				MAX_STRING,
				chemical_formula_melt[i].chemical_formula,
				MAX_STRING,
				&chemical_formula_melt[i].material.carbon,
				&chemical_formula_melt[i].material.hydrogen,
				&chemical_formula_melt[i].material.nitrogen,
				&chemical_formula_melt[i].material.oxygen,
				&chemical_formula_melt[i].material.sulfur,
				&chemical_formula_melt[i].material.chlorine,
				&chemical_formula_melt[i].number_of_bullets,
				&chemical_formula_melt[i].damage,
				&chemical_formula_melt[i].damage_per_second,
				&chemical_formula_melt[i].time,
				&attribute,
				&chemical_formula_melt[i].tag_number);
			chemical_formula_melt[i].atribute = static_cast <ATTRIBUTE>(attribute);
			chemical_formula_melt[i].make_bool = true;
		}
		return;
	}
	fclose(fp); //ÉtÉ@ÉCÉãÇï¬Ç∂ÇÈ
}

ChemicalFormulaParameter* Pouch::GetExplosion()
{
	return &select_explosion;
}

ChemicalFormulaParameter* Pouch::GetPoison()
{

	return &select_poison;
}

ChemicalFormulaParameter* Pouch::GetPararysis()
{

	return &select_pararysis;
}

ChemicalFormulaParameter* Pouch::GetHeal()
{

	return &select_heal;
}

ChemicalFormulaParameter* Pouch::GetMelt()
{

	return &select_melt;
}

ATTRIBUTE Pouch::GetAttribute()
{

	return tab;
}

int Pouch::GetCursol()
{

	return cursol;
}

bool Pouch::GetOnBool()
{

	return on_bool;
}

void Pouch::SetOnBool(bool a)
{

	on_bool = a;
}


//-----------------------------------
// íeÇå∏ÇÁÇ∑
//-----------------------------------
void Pouch::ReduceAmmo(ATTRIBUTE a)
{

	switch (a)
	{
	case ATTRIBUTE::NORMAL:
		break;
	case ATTRIBUTE::EXPLOSION:
		if (select_explosion.make_bool)
		{
			select_explosion.number_of_bullets--;
		}
		break;
	case ATTRIBUTE::MELT:
		if (select_melt.make_bool)
		{
			select_melt.number_of_bullets--;
		}
		break;
	case ATTRIBUTE::POISON:
		if (select_poison.make_bool)
		{
			select_poison.number_of_bullets--;
		}
		break;
	case ATTRIBUTE::PARALYSIS:
		if (select_pararysis.make_bool)
		{
			select_pararysis.number_of_bullets--;
		}
		break;
	case ATTRIBUTE::HEAL:
		select_heal.number_of_bullets--;
		break;
	default:
		break;
	}
}

void Pouch::SetChemicalFormulaParameter()
{
	switch (tab)
	{
	case ATTRIBUTE::EXPLOSION:
		select_explosion = chemical_formula_explosion[cursol];
		chemical_formula_explosion[cursol].make_bool = true;
		break;
	case ATTRIBUTE::MELT:
		select_melt = chemical_formula_melt[cursol];
		break;
	case ATTRIBUTE::POISON:
		select_poison = chemical_formula_poison[cursol];
		break;
	case ATTRIBUTE::PARALYSIS:
		select_pararysis = chemical_formula_pararysis[cursol];
		break;
	case ATTRIBUTE::HEAL:
		select_heal = chemical_formula_heal[cursol];
		break;
	default:
		break;
	}
}

bool Pouch::ComparisonElement(ChemicalFormulaParameter subject) const
{
	if (subject.material.hydrogen <= element[0]->GetVolume() &&
		subject.material.oxygen <= element[1]->GetVolume() &&
		subject.material.carbon <= element[2]->GetVolume() &&
		subject.material.nitrogen <= element[3]->GetVolume() &&
		subject.material.sulfur <= element[4]->GetVolume() &&
		subject.material.chlorine <= element[5]->GetVolume())
	{
		return true;
	}
	return false;
}

void Pouch::ConsumptionMaterial()
{

	int difference[PLAYER_ELEMENT];
	switch (tab)
	{
	case ATTRIBUTE::EXPLOSION:
		difference[static_cast<int>(ELEMENT_ITEM::HYDROGEN)] = element[0]->GetVolume() - chemical_formula_explosion[cursol].material.hydrogen;
		difference[static_cast<int>(ELEMENT_ITEM::OXYGEN)] = element[1]->GetVolume() - chemical_formula_explosion[cursol].material.oxygen;
		difference[static_cast<int>(ELEMENT_ITEM::CARBON)] = element[2]->GetVolume() - chemical_formula_explosion[cursol].material.carbon;
		difference[static_cast<int>(ELEMENT_ITEM::NITROGEN)] = element[3]->GetVolume() - chemical_formula_explosion[cursol].material.nitrogen;
		difference[static_cast<int>(ELEMENT_ITEM::SULFUR)] = element[4]->GetVolume() - chemical_formula_explosion[cursol].material.sulfur;
		difference[static_cast<int>(ELEMENT_ITEM::CHLORINE)] = element[5]->GetVolume() - chemical_formula_explosion[cursol].material.chlorine;
		for (int i = 0; i < PLAYER_ELEMENT - 1; i++)
		{
			element[i]->SetVolume(difference[i]);
		}
		break;
	case ATTRIBUTE::MELT:
		difference[static_cast<int>(ELEMENT_ITEM::HYDROGEN)] = element[0]->GetVolume() - chemical_formula_melt[cursol].material.hydrogen;
		difference[static_cast<int>(ELEMENT_ITEM::OXYGEN)] = element[1]->GetVolume() - chemical_formula_melt[cursol].material.oxygen;
		difference[static_cast<int>(ELEMENT_ITEM::CARBON)] = element[2]->GetVolume() - chemical_formula_melt[cursol].material.carbon;
		difference[static_cast<int>(ELEMENT_ITEM::NITROGEN)] = element[3]->GetVolume() - chemical_formula_melt[cursol].material.nitrogen;
		difference[static_cast<int>(ELEMENT_ITEM::SULFUR)] = element[4]->GetVolume() - chemical_formula_melt[cursol].material.sulfur;
		difference[static_cast<int>(ELEMENT_ITEM::CHLORINE)] = element[5]->GetVolume() - chemical_formula_melt[cursol].material.chlorine;
		for (int i = 0; i < PLAYER_ELEMENT - 1; i++)
		{
			element[i]->SetVolume(difference[i]);
		}
		break;
	case ATTRIBUTE::POISON:
		difference[static_cast<int>(ELEMENT_ITEM::HYDROGEN)] = element[0]->GetVolume() - chemical_formula_poison[cursol].material.hydrogen;
		difference[static_cast<int>(ELEMENT_ITEM::OXYGEN)] = element[1]->GetVolume() - chemical_formula_poison[cursol].material.oxygen;
		difference[static_cast<int>(ELEMENT_ITEM::CARBON)] = element[2]->GetVolume() - chemical_formula_poison[cursol].material.carbon;
		difference[static_cast<int>(ELEMENT_ITEM::NITROGEN)] = element[3]->GetVolume() - chemical_formula_poison[cursol].material.nitrogen;
		difference[static_cast<int>(ELEMENT_ITEM::SULFUR)] = element[4]->GetVolume() - chemical_formula_poison[cursol].material.sulfur;
		difference[static_cast<int>(ELEMENT_ITEM::CHLORINE)] = element[5]->GetVolume() - chemical_formula_poison[cursol].material.chlorine;
		for (int i = 0; i < PLAYER_ELEMENT - 1; i++)
		{
			element[i]->SetVolume(difference[i]);
		}
		break;
	case ATTRIBUTE::PARALYSIS:
		difference[static_cast<int>(ELEMENT_ITEM::HYDROGEN)] = element[0]->GetVolume() - chemical_formula_pararysis[cursol].material.hydrogen;
		difference[static_cast<int>(ELEMENT_ITEM::OXYGEN)] = element[1]->GetVolume() - chemical_formula_pararysis[cursol].material.oxygen;
		difference[static_cast<int>(ELEMENT_ITEM::CARBON)] = element[2]->GetVolume() - chemical_formula_pararysis[cursol].material.carbon;
		difference[static_cast<int>(ELEMENT_ITEM::NITROGEN)] = element[3]->GetVolume() - chemical_formula_pararysis[cursol].material.nitrogen;
		difference[static_cast<int>(ELEMENT_ITEM::SULFUR)] = element[4]->GetVolume() - chemical_formula_pararysis[cursol].material.sulfur;
		difference[static_cast<int>(ELEMENT_ITEM::CHLORINE)] = element[5]->GetVolume() - chemical_formula_pararysis[cursol].material.chlorine;
		for (int i = 0; i < PLAYER_ELEMENT - 1; i++)
		{
			element[i]->SetVolume(difference[i]);
		}
		break;
	case ATTRIBUTE::HEAL:
		difference[static_cast<int>(ELEMENT_ITEM::HYDROGEN)] = element[0]->GetVolume() - chemical_formula_heal[cursol].material.hydrogen;
		difference[static_cast<int>(ELEMENT_ITEM::OXYGEN)] = element[1]->GetVolume() - chemical_formula_heal[cursol].material.oxygen;
		difference[static_cast<int>(ELEMENT_ITEM::CARBON)] = element[2]->GetVolume() - chemical_formula_heal[cursol].material.carbon;
		difference[static_cast<int>(ELEMENT_ITEM::NITROGEN)] = element[3]->GetVolume() - chemical_formula_heal[cursol].material.nitrogen;
		difference[static_cast<int>(ELEMENT_ITEM::SULFUR)] = element[4]->GetVolume() - chemical_formula_heal[cursol].material.sulfur;
		difference[static_cast<int>(ELEMENT_ITEM::CHLORINE)] = element[5]->GetVolume() - chemical_formula_heal[cursol].material.chlorine;
		for (int i = 0; i < PLAYER_ELEMENT - 1; i++)
		{
			element[i]->SetVolume(difference[i]);
		}
		break;
	default:
		break;
	}
}

void Pouch::InitializeExplosion()
{
	select_explosion = { 0 };
	select_explosion.make_bool = false;	
}

void Pouch::InitializeHeal()
{
	select_heal = { 0 };
	select_heal.make_bool = false;
}

void Pouch::InitializeMelt()
{
	select_melt = { 0 };
	select_melt.make_bool = false;
}

void Pouch::InitializePararysis()
{

	select_pararysis = { 0 };
	select_pararysis.make_bool = false;
}

void Pouch::SetChemicalBullets(ChemicalFormulaParameter chemical_bullet)
{
	switch (chemical_bullet.atribute)
	{
	case ATTRIBUTE::EXPLOSION:
		select_explosion = chemical_bullet;
		break;

	case ATTRIBUTE::MELT:
		select_melt = chemical_bullet;
		break;

	case ATTRIBUTE::POISON:
		select_poison = chemical_bullet;
		break;

	case ATTRIBUTE::PARALYSIS:
		select_pararysis = chemical_bullet;
		break;

	case ATTRIBUTE::HEAL:
		select_heal = chemical_bullet;
		break;

	default:
		break;
	}
}

void Pouch::InitializePoison()
{
	select_poison = { 0 };
	select_poison.make_bool = false;
}