#include "MovingSmallTrap.h"
#include "Player.h"
#include "Collision.h"

MovingSmallTrap::MovingSmallTrap(float sx, float sy)
{
	SImage = LoadGraph("data/image/Smallhari.png"); // 小さい針画像
	x = sx;
	y = sy;
}

MovingSmallTrap::~MovingSmallTrap()
{
}

void MovingSmallTrap::Update() 
{
	Player* player = FindGameObject<Player>();
	if (!player) return;

	float cx, cy, cr;
	player->GetHitCircle(cx, cy, cr);
	VECTOR center = VGet(cx, cy, 0.0f);

	// 中心座標
	float trapCx = x + width * 0.5f;
	float playerCx = cx;

	if (!moved) 
	{
		// 左に1マス来た
		if (playerCx < trapCx - tile && playerCx > trapCx - tile * 1.5f)
		{
			x -= tile;
			moved = true;
		}
		// 右に1マス来た
		else if (playerCx > trapCx + tile && playerCx < trapCx + tile * 1.5f)
		{
			x += tile;
			moved = true;
		}
	}


	// トラップの当たり判定(三角)
	float sx = x;
	float sy = y;
	VECTOR t1{}, t2{}, t3{};
	t1 = VGet(sx, sy + height, 0.0f); // 左下
	t2 = VGet(sx + width, sy + height, 0.0f); // 右下
	t3 = VGet(sx + width / 2.0f, sy, 0.0f); // 上の先端

	if (HitCheck_Circle_Triangle(center, cr, t1, t2, t3))
	{
		player->ForceDie();
		player->SetDead();
	}

	
}

void MovingSmallTrap::Draw() 
{
	// 中心座標（1タイル 64x64 前提）
	float cx = x + width * 0.5f;
	float cy = y + height * 0.5f;
	// 拡大率 1.0（そのままの大きさ）
	DrawRotaGraph(static_cast<int>(cx),static_cast<int>(cy),1.0,0,SImage,TRUE);
}
