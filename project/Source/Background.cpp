#include "Background.h"
#include <DxLib.h>

Background::Background()
{
	bgImage = LoadGraph("data/image/kabe.png");
	SetDrawOrder(1000);
	sw = 0;
	sh = 0;
	GetDrawScreenSize(&sw, &sh);
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
	DrawExtendGraph(0, 0, sw, sh, bgImage, FALSE);
}
