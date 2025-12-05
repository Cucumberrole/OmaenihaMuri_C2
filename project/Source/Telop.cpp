#include <DxLib.h>
#include "Telop.h"
#include "Player.h"
#include "Field.h"
#include "SmallTrap.h"

extern bool g_isPlayerTouchedTrap;

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
        DrawString(160, 500, "‚ ‚Ù", GetColor(255, 255, 255));
    }
}


