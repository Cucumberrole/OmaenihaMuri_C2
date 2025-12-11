#include "Background.h"
#include <DxLib.h>

Background::Background()
{
	bgImage = LoadGraph("data/image/kabe.png");
	SetDrawOrder(1000);
}

Background::~Background()
{
	DeleteGraph(bgImage);
}

void Background::Update()
{

}

void Background::Draw()
{
	DrawGraph(0, 0, bgImage, FALSE);
}
