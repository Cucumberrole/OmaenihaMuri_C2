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
    if (TouchedTrap1)//MovingSmallTrap
    {
        DrawFormatString(360, 240, GetColor(255, 255, 255), "Ž€‚ñ‚¶‚á‚Á‚½w", FALSE);
    }
    if (TouchedTrap2)//Dokan
    {
        displayTimer -= 1.0f / 60.0f;
        if (displayTimer <= 0.0f)
        {
            TouchedTrap2 = false;
            displayTimer = 3.0f;
        }
        DrawFormatString(1700, 200, GetColor(255, 255, 255), "‚â‚Á‚Ù[", FALSE);
    }
    if (TouchedTrap3)//FallingFloor
    {
        displayTimer -= 1.0f / 60.0f; 
        if (displayTimer <= 0.0f)
        {
            TouchedTrap3 = false;
            displayTimer = 3.0f;
        }
        DrawFormatString(1000, 500, GetColor(255, 255, 255), "‰³‚—", FALSE);
    }
    if (TouchedTrap4)//FallingFloor
    {
        displayTimer -= 1.0f / 60.0f;
        if (displayTimer <= 0.0f)
        {
            TouchedTrap3 = false;
            displayTimer = 3.0f;
        }
        DrawFormatString(1000, 500, GetColor(255, 255, 255), "‚æ‚í‚—", FALSE);
    }
    if (TouchedTrap5)
    {
        DrawFormatString(360, 75, GetColor(255, 255, 255), "I", FALSE);
    }
    
}

void Telop::ShowTrap3Message(float duration)
{
    if (TouchedTrap3) return; // ‚·‚Å‚É•\Ž¦Ï‚Ý‚È‚ç–³Ž‹
    TouchedTrap3 = true;
    displayTimer = duration;
}


