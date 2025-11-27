#include <DxLib.h>
#include "SmallTrap.h"
#include "Player.h"

SmallTrap::SmallTrap(int sx, int sy)
{
	SImage = LoadGraph("data/image/Smallhari.png"); // è¨Ç≥Ç¢êjâÊëú
	x = sx;
	y = sy;
	width = 16;
	height = 16;

}

SmallTrap::~SmallTrap()
{
	DeleteGraph(SImage);
}



void SmallTrap::Update()
{
	//----------------------------------------
	// ÉvÉåÉCÉÑÅ[Ç∆è’ìÀ
	//----------------------------------------
	Player* player = FindGameObject<Player>();
	if (player)
	{
		float px = player->GetX();
		float py = player->GetY();
		float pw = 64, ph = 64;

		bool hit =
			(x < px + pw) &&
			(x + width > px) &&
			(y < py + ph) &&
			(y + height > py);

		if (hit)
		{
			player->ForceDie();
			player->SetDead();
		}
	}
}



void SmallTrap::Draw()
{
	DrawRectGraph(
		static_cast<int>(x),
		static_cast<int>(y),
		0, 0, width, height,
		SImage,
		TRUE
	);
	DrawCircle(x + 8, y + 8, 8, GetColor(0, 0, 255), FALSE);
}