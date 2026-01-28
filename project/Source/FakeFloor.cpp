#include "FakeFloor.h"
#include "Player.h"
#include <DxLib.h>

bool FakeFloor::CheckHitBall(float px, float py, float pw, float ph)
{
	if (px + pw <= x  && px >= x + width && py + ph <= y && py >= y + height) 
		return true;
}

FakeFloor::FakeFloor(int sx, int sy)
{
	hImage = DxLib::LoadGraph("data/image/NewBlock.png");
	x = sx;
	y = sy;
}

FakeFloor::~FakeFloor()
{
	DeleteGraph(hImage);
}

void FakeFloor::Update()
{
}

void FakeFloor::Draw()
{
	DxLib::DrawRectGraph(static_cast<int>(x), static_cast<int>(y), 0, 0, 64, 64, hImage, TRUE);
}