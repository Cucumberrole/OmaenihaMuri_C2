#include "FlyingSpike.h"
#include "Player.h"
#include "Field.h"
#include <DxLib.h>

FlyingSpike::FlyingSpike(float startX, float startY, float speed)
{
	hImage = LoadGraph("data/image/hariLeft.png");

	x = startX;
	y = startY;
	this->speed = speed;

	width = 64;
	height = 64;
}

FlyingSpike::~FlyingSpike()
{
	DeleteGraph(hImage);
}

void FlyingSpike::Update()
{
	// 針の移動
	x += speed;

	// --- プレイヤー取得 ---
	Player* player = FindGameObject<Player>();
	if (player)
	{
		float px = player->GetX();
		float py = player->GetY();
		float pw = 64;
		float ph = 64;

		// --- AABB 当たり判定 ---
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

	// --- 壁に当たったら消える ---
	Field* field = FindGameObject<Field>();
	if (field)
	{
		int tileX = int(x) / 64;
		int tileY = int(y) / 64;

		if (field->IsBlock(tileX, tileY))  // 壁判定
		{
			DestroyMe();
		}
	}

	// 画面外で消える
	if (x < -64 || x > 5000)
	{
		DestroyMe();
	}
}

void FlyingSpike::Draw()
{
	DrawGraph((int)x, (int)y, hImage, TRUE);
}