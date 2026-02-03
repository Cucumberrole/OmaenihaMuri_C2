#include <DxLib.h>
#include "Telop.h"
#include "Player.h"
#include "Field.h"
#include "SmallTrap.h"

Telop::Telop()
{

}

Telop::~Telop()
{
}

void Telop::StartDeathTelop(Player* p)
{
	player = p;

	if (p)
	{
		deathTelopX_ = p->GetX();
		deathTelopY_ = p->GetY();
	}

	// 表示フラグ
	ShowDeathTelop = true;
	showDeathTelop_ = true;

	const char* messages[] = {
		"やめようよ",
		"痛いよ",
		"人の心とかないんか",
		"（´・ω・｀）",
		"(´;ω;｀)",
		"(　ﾟдﾟ　)"
	};

	int count = (int)(sizeof(messages) / sizeof(messages[0]));
	int index = rand() % count;
	deathText = messages[index];

	// 死亡テロップ専用タイマー
	deathTelopTimer_ = 3.0f;
}

void Telop::Update()
{
	// 死亡テロップ
	if (showDeathTelop_)
	{
		deathTelopTimer_ -= 1.0f / 60.0f;
		if (deathTelopTimer_ <= 0.0f)
		{
			showDeathTelop_ = false;
			ShowDeathTelop = false;
		}
	}
	if (!ShowDeathTelop) return;

	displayTimer -= 1.0f / 60.0f;
	if (displayTimer <= 0.0f)
	{
		ShowDeathTelop = false;
	}
}

void Telop::Draw()
{
	if (TouchedTrap1)//MovingSmallTrap
	{
		DrawFormatString(360, 240, GetColor(255, 255, 255), "死んじゃったw", FALSE);
	}
	if (TouchedTrap2)//Dokan
	{
		displayTimer -= 1.0f / 60.0f;
		if (displayTimer <= 0.0f)
		{
			TouchedTrap2 = false;
			displayTimer = 3.0f;
		}
		DrawFormatString(1700, 200, GetColor(255, 255, 255), "やっほー", FALSE);
	}

	if (TouchedTrap3)//FallingFloor右下
	{
		DrawFormatString(1280, 576, GetColor(255, 255, 255), "乙ｗ", FALSE);
	}

	if (TouchedTrap4)//FallingFloor右上
	{
		DrawFormatString(1088, 256, GetColor(255, 255, 255), "ｗｗｗｗｗ", FALSE);
	}

	if (TouchedTrap5)//FallingFloor左上
	{
		DrawFormatString(128, 256, GetColor(255, 255, 255), "おしい！！！！", FALSE);
	}

	if (TouchedTrap6)//VanishingFloor
	{
		DrawFormatString(1152, 640, GetColor(255, 255, 255), "終＼(＾o＾)／", FALSE);
	}

	if (ShowDeathTelop && player)
	{
		float baseX = player->IsDead() ? player->GetDeathDrawX() : player->GetX();
		float baseY = player->IsDead() ? player->GetDeathDrawY() : player->GetY();

		float drawX = baseX + offsetX;
		float drawY = baseY + offsetY;

		DrawFormatString(
			(int)drawX,
			(int)drawY,
			GetColor(255, 255, 255),
			deathText.c_str(),
			FALSE
		);
	}

}

void Telop::ShowTrap3Message(float duration)
{
	if (TouchedTrap3) return; // すでに表示済みなら無視
	TouchedTrap3 = true;
	displayTimer = duration;
}




