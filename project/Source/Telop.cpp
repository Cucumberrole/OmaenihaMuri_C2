#include <DxLib.h>
#include "Telop.h"
#include "Player.h"
#include "Field.h"
#include "SmallTrap.h"

Telop::Telop()
{
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
        DrawString(550, 200, "‚ ‚Ù", GetColor(255, 255, 255));
    }
}


