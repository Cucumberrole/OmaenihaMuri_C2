#include "FakeFloor.h"
#include "Player.h"
#include <DxLib.h>

bool FakeFloor::CheckHitBall(float px, float py, float pw, float ph)
{
	if (vanished_) return false;

	// AABB（矩形同士）当たり判定：重なっていればヒット
	const bool hit =
		!(px + pw <= x ||
			px >= x + width ||
			py + ph <= y ||
			py >= y + height);

	if (hit)
	{
		vanished_ = true; // 触れた瞬間に消す
		return true;
	}
	return false;
}

FakeFloor::FakeFloor(int sx, int sy)
{
	hImage = LoadGraph("data/image/NewBlock.png");
	x = (float)sx;
	y = (float)sy;
}

FakeFloor::~FakeFloor()
{
	DeleteGraph(hImage);
}

void FakeFloor::Update()
{
	// 消えた後は特に何もしない（必要ならここで削除依頼など）
}

void FakeFloor::Draw()
{
	if (vanished_) return;
	DrawRectGraph((int)x, (int)y, 0, 0, 64, 64, hImage, TRUE);
}
