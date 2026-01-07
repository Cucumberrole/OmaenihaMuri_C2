#include <DxLib.h>
#include "SmallTrap.h"
#include "Player.h"
#include "Telop.h"




SmallTrap::SmallTrap(int sx, int sy)
{
	SImage = LoadGraph("data/image/Smallhari.png"); // 小さい針画像
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
	// プレイヤーと衝突
	//----------------------------------------
	Telop* telop = FindGameObject<Telop>();
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
			//telop->TouchedTrap = true;

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
	//DrawBox(
	//	static_cast<int>(x),
	//	static_cast<int>(y),
	//	static_cast<int>(x + width),
	//	static_cast<int>(y + height),
	//	GetColor(255, 0, 0), // 赤色
	//	FALSE                // 塗りつぶしなし
	//);
}