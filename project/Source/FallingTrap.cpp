#include "FallingTrap.h"
#include "Player.h"
#include "Field.h"
#include <vector>
#include <DxLib.h>

FallingTrap::FallingTrap(int sx, int sy)
{
	hImage = LoadGraph("data/image/New Blo.png");
	x = sx;
	y = sy;

	velocityY = 0.0f;
	isFalling = false;
	isLanded = false;
	gravity = 0.8f;
	SetDrawOrder(-1); // 描画順位
}

FallingTrap::~FallingTrap()
{
	DeleteGraph(hImage);
}

void FallingTrap::Update()
{
	if (isLanded) return; // もう止まっているなら何もしない

	Player* player = FindGameObject<Player>();
	if (player == nullptr) return;

	float px = player->GetX();
	float py = player->GetY();

	// --- プレイヤーが真下を通過したら落下開始 ---
	if (!isFalling && px + 64 > x && px < x + 64 && py > y)
	{
		StartFalling();
	}

	// --- 落下処理 ---
	if (isFalling)
	{
		velocityY += gravity;
		y += velocityY;

		// --- 下のFieldブロックに当たったら停止 ---
		Field* field = FindGameObject<Field>();
		if (field)
		{
			int push1 = field->HitCheckDown(x + 5, y + 64);
			int push2 = field->HitCheckDown(x + 59, y + 64);
			int push = max(push1, push2);

			if (push > 0)
			{
				y -= push;        // 押し戻して止める
				velocityY = 0.0f;
				isFalling = false;
				isLanded = true;  // 着地完了
			}
		}
	}
	// --- 落下中のプレイヤー潰し判定 ---
	if (isFalling)
	{
		// 床の矩形
		float fx1 = x;
		float fx2 = x + 64;
		float fy1 = y;
		float fy2 = y + 64;

		// プレイヤーの矩形
		float px1 = px;
		float px2 = px + 64;
		float py1 = py;
		float py2 = py + 64;

		// --- 条件：床が上からプレイヤーに重なったら即死 ---
		bool hit =
			(fx1 < px2) && (fx2 > px1) &&  // 横重なり
			(fy1 < py2) && (fy2 > py1) &&  // 縦重なり
			(velocityY > 0);               // 落下中のみ判定

		if (hit)
		{
			player->ForceDie();
			player->SetDead();
		}
	}

}

void FallingTrap::StartFalling()
{
	isFalling = true;
	velocityY = 0.0f;
}

void FallingTrap::Draw()
{
	DrawRectGraph(static_cast<int>(x), static_cast<int>(y), 0, 0, 64, 64, hImage, TRUE);
}

int FallingTrap::HitCheckRight(int px, int py)
{
	// px,py → プレイヤーの判定点
	if (py < y || py >= y + 64) return 0; // 縦方向が重なっていない

	int localX = px - x;
	if (localX >= 0 && localX < 64)
	{
		return localX + 1;
	}
	return 0;
}

int FallingTrap::HitCheckLeft(int px, int py)
{
	if (py < y || py >= y + 64) return 0;

	int localX = px - x;
	if (localX >= 0 && localX < 64)
	{
		return 64 - localX;
	}
	return 0;
}

int FallingTrap::HitCheckDown(int px, int py)
{
	// px が床の横範囲にない
	if (px < x || px >= x + 64) return 0;

	int localY = py - y;
	if (localY >= 0 && localY < 64)
	{
		return localY + 1;
	}
	return 0;
}

int FallingTrap::HitCheckUp(int px, int py)
{
	if (px < x || px >= x + 64) return 0;

	int localY = py - y;
	if (localY >= 0 && localY < 64)
	{
		return 64 - localY;
	}
	return 0;
}