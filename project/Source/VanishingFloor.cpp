#include "VanishingFloor.h"
#include "Player.h"
#include <DxLib.h>

VanishingFloor::VanishingFloor(int sx, int sy)
{
	hImage = LoadGraph("data/image/New Blo.png");  // 床画像
	x = (float)sx;
	y = (float)sy;

	isActive = true;        // 最初は存在している
	vanishRange = 100.0f;   // プレイヤーが100px以内で消える
}

VanishingFloor::~VanishingFloor()
{
	DeleteGraph(hImage);
}

void VanishingFloor::Update()
{
	if (!isActive) return;

	Player* player = FindGameObject<Player>();
	if (!player) return;

	float px = player->GetX();
	float py = player->GetY();

	// --- プレイヤーとの距離 ---
	float dx = px - (x + 32);
	float dy = py - (y + 32);
	float dist = dx * dx + dy * dy;

	if (dist < vanishRange * vanishRange)
	{
		isActive = false;   // 床が消える
	}
}

void VanishingFloor::Draw()
{
	if (isActive)
	{
		DrawRectGraph((int)x, (int)y, 0, 0, 64, 64, hImage, TRUE);
	}
}