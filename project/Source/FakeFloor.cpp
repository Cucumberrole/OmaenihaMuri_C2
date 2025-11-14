#include "FakeFloor.h"
#include <DxLib.h>

FakeFloor::FakeFloor(int sx, int sy)
{
	hImage = LoadGraph("data/image/New Blo.png");
	x = static_cast<float>(sx);
	y = static_cast<float>(sy);
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