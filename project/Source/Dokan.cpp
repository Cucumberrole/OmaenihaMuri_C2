#include "Dokan.h"

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
	DrawRectGraph(static_cast<int>(x), static_cast<int>(y), 0, 0, 64, 64, dImage, TRUE);
}