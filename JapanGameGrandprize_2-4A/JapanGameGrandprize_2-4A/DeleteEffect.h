#pragma once
#include "Define.h"
#include "DxLib.h"


class DeleteEffect
{
private:
	int Tick;
	bool effect_end;
	Delete_Effect delete_effect;

	Location location;
	Delete_Effect exp;
	Delete_Effect mel;
	Delete_Effect par;
	Delete_Effect poi;
	ATTRIBUTE attribute;

public:
	DeleteEffect();
	DeleteEffect(Location,ATTRIBUTE,bool);
	~DeleteEffect();

	void Draw()const;
	void Update();

	bool GetEffectEnd();
};