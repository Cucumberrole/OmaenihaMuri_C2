#include <DxLib.h>
#include "Telop.h"
#include "Player.h"
#include "Field.h"
#include "SmallTrap.h"

Telop::Telop()
{
    a = LoadGraph("data/Font/Ç†...png");
    b = LoadGraph("data/Font/Ç¶ÅH.png");
    c = LoadGraph("data/Font/â≥.png");
    d = LoadGraph("data/Font/é„.png");
    e = LoadGraph("data/Font/èI.png");
    f = LoadGraph("data/Font/ê^ñ ñ⁄Ç…Ç‚ÇÎÇ§ÇÀ.png");
    g = LoadGraph("data/Font/êßçÏé“.png");
}

Telop::~Telop()
{
}

void Telop::Update()
{
}

void Telop::Draw()
{
    if (g_isPlayerTouchedTrap)
    {
        DrawGraph(550, 200, c, FALSE);
    }
}


