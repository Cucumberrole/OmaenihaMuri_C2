#include "FlyingSpike.h"
#include "Player.h"
#include "Field.h"
#include "Telop.h"
#include "Collision.h"
#include <DxLib.h>

static const char* Text = nullptr;

FlyingSpike::FlyingSpike(float startX, float startY, float speed)
{
	hImage = LoadGraph("data/image/hariLeft.png");

	x = startX;
	y = startY;
	this->speed = speed;

	width = 64;
	height = 64;

	if (Text == nullptr)
	{
		const char* messages[] = {
			"よけろ",
			"当たるよ？",
			"遅い",
			"見えてる？",
			"やっほー"
		};

		int count = sizeof(messages) / sizeof(messages[0]);
		Text = messages[GetRand(count - 1)];
	}
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
		float pw = 64.0f;
		float ph = 64.0f;

		// プレイヤーの中心と半径
		VECTOR center = VGet(px + pw * 0.5f, py + ph * 0.5f, 0.0f);
		float  radius = pw * 0.5f;

		// 左向きトゲの三角形
		float sx = x;
		float sy = y;

		VECTOR t1 = VGet(sx + width, sy, 0.0f);          // 右上
		VECTOR t2 = VGet(sx + width, sy + height, 0.0f); // 右下
		VECTOR t3 = VGet(sx, sy + height / 2, 0.0f); // 左側の先端

		if (HitCheck_Circle_Triangle(center, radius, t1, t2, t3))
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
	DrawString((int)(x + width),(int)(y - 20),Text,GetColor(255, 255, 255));
}