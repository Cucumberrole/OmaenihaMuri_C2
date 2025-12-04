#include "FallingSpike.h"
#include "Field.h"
#include "Player.h"
#include <DxLib.h>

FallingSpike::FallingSpike(int sx, int sy)
{
	hImage = LoadGraph("data/image/hariBottom.png");

	x = sx;
	y = sy;

	vy = 0;
	gravity = 0.8f;
	landed = false;
}

FallingSpike::~FallingSpike()
{
	DeleteGraph(hImage);
}

void FallingSpike::Update()
{
	if (!landed)
	{
		vy += gravity;
		y += vy;

		// 地面に衝突判定
		Field* field = FindGameObject<Field>();
		if (field)
		{
			int tx = (x + 32) / 64;
			int ty = (y + 64) / 64;

			if (field->IsBlock(tx, ty))
			{
				y = ty * 64 - 64;
				landed = true;
				vy = 0;
			}
		}
	}

	// プレイヤー衝突判定
	Player* player = FindGameObject<Player>();
	if (player)
	{
		float px = player->GetX();
		float py = player->GetY();

		bool hit =
			x < px + 64 &&
			x + 64 > px &&
			y < py + 64 &&
			y + 64 > py;

		if (hit)
		{
			player->ForceDie();
			player->SetDead();
		}
	}
}

void FallingSpike::Draw()
{
	DrawGraph((int)x, (int)y, hImage, TRUE);
}