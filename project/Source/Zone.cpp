#include "Zone.h"
#include "Player.h"
#include <DxLib.h>


Zone::Zone(float sx, float sy, float w, float h)
{
	x = sx;
	y = sy;
	width = w;
	height = h;
}

Zone::~Zone()
{
}

void Zone::Update()
{
	Player* player = FindGameObject<Player>();
	if (!player)return;

	float px = player->GetX();
	float py = player->GetY();

	bool hit =
		px + 64 > x &&
		px < x + width &&
		py + 64 >y &&
		py < y + height;

	if (hit)
	{
		player->SetReverse(true);
	}
}

void Zone::Draw()
{
}
