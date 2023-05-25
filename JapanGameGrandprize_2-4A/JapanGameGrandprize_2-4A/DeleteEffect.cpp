#include "DeleteEffect.h"
#include "CameraWork.h"

DeleteEffect::DeleteEffect()
{
	location = { 0 };
	attribute = ATTRIBUTE::NORMAL;
	
	

}

DeleteEffect::DeleteEffect(Location location, ATTRIBUTE attribute,bool effect_end)
{
	
	this->location = location;
	this->attribute = attribute;
	this->effect_end = effect_end;
	Tick = 0;
	switch (this->attribute)
	{
	case ATTRIBUTE::NORMAL:
		break;
	case ATTRIBUTE::EXPLOSION:
		LoadDivGraph("Images/Player/exp_2.png", 8, 8, 1, 120, 120, exp.Delete_image_array/*, TRUE, FALSE, FALSE*/);
		exp.Delete_display_permit = FALSE;
		exp.Delete_frame = 0;
		exp.Delete_x = location.x;
		exp.Delete_y = location.y;
		break;
	case ATTRIBUTE::MELT:
		LoadDivGraph("Images/Player/mel_2.png", 8, 8, 1, 120, 120, mel.Delete_image_array, TRUE, FALSE, FALSE);
		mel.Delete_display_permit = false;
		mel.Delete_frame = 0;
		mel.Delete_x = location.x;
		mel.Delete_y = location.y;
		break;
	case ATTRIBUTE::POISON:
		LoadDivGraph("Images/Player/poi_2.png", 8, 8, 1, 120, 120, poi.Delete_image_array, TRUE, FALSE, FALSE);
		poi.Delete_display_permit = false;
		poi.Delete_frame = 0;
		poi.Delete_x = location.x;
		poi.Delete_y = location.y;
		break;
	case ATTRIBUTE::PARALYSIS:
		LoadDivGraph("Images/Player/pal_2.png", 8, 8, 1, 120, 120, par.Delete_image_array, TRUE, FALSE, FALSE);
		par.Delete_display_permit = false;
		par.Delete_frame = 0;
		par.Delete_x = location.x;
		par.Delete_y = location.y;
		break;
	default:
		break;
	}
}
	

DeleteEffect::~DeleteEffect()
{

}


void DeleteEffect::Draw()const
{
	float scrool_x = CameraWork::GetCamera().x;
	float scrool_y = CameraWork::GetCamera().y;
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	switch (attribute)
	{
	case ATTRIBUTE::NORMAL:
		break;
	case ATTRIBUTE::EXPLOSION:
		DrawRotaGraph(exp.Delete_x - scrool_x, exp.Delete_y - scrool_y, 1, 0, exp.Delete_image_array[exp.Delete_frame], TRUE, FALSE);
		break;
	case ATTRIBUTE::MELT:
		DrawRotaGraph(mel.Delete_x - scrool_x, mel.Delete_y - scrool_y, 1, 0, mel.Delete_image_array[mel.Delete_frame], TRUE, FALSE, FALSE);
		break;
	case ATTRIBUTE::POISON:
		DrawRotaGraph(poi.Delete_x - scrool_x, poi.Delete_y - scrool_y, 1, 0, poi.Delete_image_array[poi.Delete_frame], TRUE, FALSE, FALSE);
		break;
	case ATTRIBUTE::PARALYSIS:
		DrawRotaGraph(par.Delete_x - scrool_x, par.Delete_y - scrool_y, 1, 0, par.Delete_image_array[par.Delete_frame], TRUE, FALSE, FALSE);
		break;
	default:
		break;
	}
}

void DeleteEffect::Update()
{
	switch (attribute)
	{
	case ATTRIBUTE::NORMAL:
		effect_end = true;
		break;
	case ATTRIBUTE::EXPLOSION:
		if (effect_end == FALSE) {
			if (Tick % 2 == 0)
			{
				exp.Delete_frame++;
			}
			if (exp.Delete_frame > 8)
			{
				exp.Delete_display_permit = FALSE;
				effect_end = TRUE;
			}
		}
		break;
	case ATTRIBUTE::MELT:
		if (effect_end == false) {
			if (Tick % 3 == 0) {
				mel.Delete_frame++;
			}
			if (mel.Delete_frame > 8) {
				mel.Delete_display_permit = FALSE;
				mel.Delete_frame = 0;
				effect_end = TRUE;
				Tick = 0;
			}
		}
		break;
	case ATTRIBUTE::POISON:
		if (effect_end == false) {
			if (Tick % 3 == 0) {
				poi.Delete_frame++;
			}
			if (poi.Delete_frame > 8) {
				poi.Delete_display_permit = FALSE;
				poi.Delete_frame = 0;
				effect_end = TRUE;
				Tick = 0;
			}
		}
		break;
	case ATTRIBUTE::PARALYSIS:
		if (effect_end == false) {
			if (Tick % 3 == 0) {
				par.Delete_frame++;
			}
			if (par.Delete_frame > 8) {
				par.Delete_display_permit = FALSE;
				par.Delete_frame = 0;
				effect_end = TRUE;
				Tick = 0;
			}
		}
		break;
	default:
		break;
	}	
}


bool DeleteEffect::GetEffectEnd()
{
	return effect_end;
}