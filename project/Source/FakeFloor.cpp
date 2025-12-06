#include "FakeFloor.h"
#include "Player.h"
#include <DxLib.h>

FakeFloor::FakeFloor(int sx, int sy)
{
	hImage = LoadGraph("data/image/NewBlock.png");
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
	DrawRectGraph(static_cast<int>(x), static_cast<int>(y), 0, 0, 64, 64, hImage, TRUE);
}