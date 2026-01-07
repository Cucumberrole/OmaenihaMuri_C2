#include <DxLib.h>
#include "Telop.h"
#include "Player.h"
#include "Field.h"
#include "SmallTrap.h"

Telop::Telop()
{
   /* a = LoadGraph("data/Font/あっ.png");
    b = LoadGraph("data/Font/え？.png");
    c = LoadGraph("data/Font/乙.png");
    d = LoadGraph("data/Font/弱.png");
    e = LoadGraph("data/Font/終.png");
    f = LoadGraph("data/Font/真面目にやろうね.png");
    g = LoadGraph("data/Font/制作者爆笑中.png");
    h = LoadGraph("data/Font/僕の気持ち考えたことある？");*/
}

Telop::~Telop()
{
}

void Telop::Update()
{
}

void Telop::Draw()
{
    if (TouchedTrap1)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "あっｗ", FALSE);
    }
    if (TouchedTrap2)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "えｗ", FALSE);
    }
    if (TouchedTrap3)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "乙ｗ", FALSE);
    }
    if (TouchedTrap4)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "よわｗ", FALSE);
    }
    if (TouchedTrap5)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "終", FALSE);
    }
    if (TouchedTrap6)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "真面目にやろうねｗ", FALSE);
    }
    if (TouchedTrap7)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "制作者爆笑中", FALSE);
    }
    if (TouchedTrap8)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "僕の気持ち考えたことある？", FALSE);
    }
    if (TouchedTrap9)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "こんにちは^^", FALSE);
    }
    if (TouchedTrap10)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "", FALSE);
    }
}


