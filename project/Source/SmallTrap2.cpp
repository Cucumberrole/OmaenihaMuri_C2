#include <DxLib.h>
#include "SmallTrap2.h"
#include "Player.h"
#include "Telop.h"


SmallTrap2::SmallTrap2(int sx, int sy)
{
	SImage = LoadGraph("data/image/SmallTrap.png"); // 小さい針画像
}

SmallTrap2::~SmallTrap2()
{
	DeleteGraph(SImage);
}

void SmallTrap2::Update()
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
			telop->TouchedTrap1 = true;

			player->ForceDie();
			player->SetDead();
		}
	}
}

void SmallTrap2::Draw()
{
	// 拡大率 1.0（そのままの大きさ）
	DrawRotaGraph(
		static_cast<int>(cx),
		static_cast<int>(cy),
		1.0,
		angle,
		SImage,
		TRUE
	);
}

