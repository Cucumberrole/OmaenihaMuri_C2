#include "Dokan.h"
#include "Screen.h"

Dokan::Dokan(int sx, int sy)
{
	dImage = LoadGraph("data/image/dokan.png");
	x = static_cast<float>(sx);
	y = static_cast<float>(sy);
}

Dokan::~Dokan()
{
	DeleteGraph(dImage);
}

void Dokan::Update()
{

}

void Dokan::Draw()
{
	//DrawRectGraph(static_cast<int>(x), static_cast<int>(y), 0, 0, 500, 500, dImage, TRUE);
	DrawExtendGraph(x, y, x + 64, y + 64, dImage, TRUE);
	//DrawRotaGraph2(x, y, 500 / 2, 500 / 2, 0.125f, 3.14, dImage, TRUE);

}