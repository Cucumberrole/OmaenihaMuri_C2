#include "RollingBall.h"
#include "Field.h"
#include "Player.h"
#include <DxLib.h>

RollingBall::RollingBall(float sx, float sy, float dir)
{
	hImage = LoadGraph("data/image/ball.png");

	size = 64;  // ← 表示と当たり判定を64に固定！

	x = sx;
	y = sy - size;

	vx = 5.0f * dir;
	vy = 0.0f;
	gravity = 0.4f;
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
	// 足元のタイル調査（穴判定）
	//----------------------------------------
	int tx = int((x + size / 2) / 64);   // 中心 X
	int ty = int((y + size) / 64);       // 足元 Y

	if (field && field->IsBlock(tx, ty))
	{
		// 床に乗った
		y = ty * 64 - size;
		vy = 0;
	}

	//----------------------------------------
	// 横移動
	//----------------------------------------
	x += vx;

	//----------------------------------------
	// 壁に当たったら反転
	//----------------------------------------
	int cx = int((x + (vx > 0 ? size : 0)) / 64);
	int cy = int((y + size / 2) / 64);

	if (field && field->IsBlock(cx, cy))
	{
		Bounce();
	}

	//----------------------------------------
	// プレイヤーと衝突
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
			player->ForceDie();
			player->SetDead();
		}
	}
}

void RollingBall::Bounce()
{
	vx = -vx;
}

void RollingBall::Draw()
{
	// 500px → 64px に縮小
	DrawExtendGraph(
		(int)x,          // 左上X
		(int)y,          // 左上Y
		(int)(x + size), // 右下X
		(int)(y + size), // 右下Y
		hImage, TRUE
	);
}
