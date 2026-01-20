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
    if (TouchedTrap1)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "Ç†Ç¡Çó", FALSE);
    }
    if (TouchedTrap2)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "Ç‚Ç¡ÇŸÅ[", FALSE);
    }
    if (TouchedTrap3)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "â≥Çó", FALSE);
    }
    if (TouchedTrap4)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "ÇÊÇÌÇó", FALSE);
    }
    if (TouchedTrap5)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "èI", FALSE);
    }
    
}


