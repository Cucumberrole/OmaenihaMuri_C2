#include "Dokan2.h"
#include "Screen.h"

Dokan2::Dokan2(int sx, int sy)
{
	dImage = LoadGraph("data/image/dokanBottom.png");
	x = static_cast<float>(sx);
	y = static_cast<float>(sy);
}

Dokan2::~Dokan2()
{
	DeleteGraph(dImage);
}

void Dokan2::Update()
{

}

void Dokan2::Draw()
{
	//DrawRectGraph(static_cast<int>(x), static_cast<int>(y), 0, 0, 500, 500, dImage, TRUE);
	DrawExtendGraph(x, y, x + 64, y + 64, dImage, TRUE);

}