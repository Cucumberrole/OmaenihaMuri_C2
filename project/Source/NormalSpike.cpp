#include "NormalSpike.h"

NormalSpike::NormalSpike(int sx, int sy)
{
	hImage = LoadGraph("data/image/hari.png");
	x = sx;
	y = sy;
	width = 64;
	height = 64;

}

NormalSpike::~NormalSpike()
{
	DeleteGraph(hImage);
}

void NormalSpike::Update()
{

}

void NormalSpike::Draw()
{

}