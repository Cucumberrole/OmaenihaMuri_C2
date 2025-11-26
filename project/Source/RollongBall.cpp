#include "RollingBall.h"
#include "Field.h"
#include "Player.h"
#include <DxLib.h>

RollingBall::RollingBall(float sx, float sy, float dir)
{
	hImage = LoadGraph("data/image/ball.png");

	x = sx;
	y = sy;

	vx = 5.0f * dir;   // dir = +1 で右、-1 で左
	vy = 0.0f;
	gravity = 0.4f;

	size = 48; // 鉄球サイズ
}

RollingBall::~RollingBall()
{
	DeleteGraph(hImage);
}

void RollingBall::Update()
{
	Field* field = FindGameObject<Field>();

	//----------------------------------------
	// 重力
	//----------------------------------------
	vy += gravity;
	y += vy;

	//----------------------------------------
	// 穴なら落下し続ける
	//----------------------------------------
	int tx = int((x + size / 2) / 64);
	int ty = int((y + size) / 64);

	if (!field->IsBlock(tx, ty))
	{
		// 穴なので落下継続
	}
	else
	{
		// 床に着地
		y = ty * 64 - size;
		vy = 0;
	}

	//----------------------------------------
	// 横移動
	//----------------------------------------
	x += vx;

	//----------------------------------------
	// 壁にぶつかったら反転
	//----------------------------------------
	int cx = int((x + (vx > 0 ? size : 0)) / 64);
	int cy = int((y + size / 2) / 64);

	if (field->IsBlock(cx, cy))
	{
		Bounce();
	}

	//----------------------------------------
	// プレイヤーとの衝突判定
	//----------------------------------------
	Player* player = FindGameObject<Player>();
	if (player)
	{
		float px = player->GetX();
		float py = player->GetY();
		float pw = 64, ph = 64;

		bool hit =
			(x < px + pw) &&
			(x + size > px) &&
			(y < py + ph) &&
			(y + size > py);

		if (hit)
		{
			player->DestroyMe();
		}
	}
}

void RollingBall::Bounce()
{
	vx = -vx;  // 反転
}

void RollingBall::Draw()
{
	DrawGraph((int)x, (int)y, hImage, TRUE);
}