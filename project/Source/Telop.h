#pragma once
#include "Player.h"
#include "../Library/GameObject.h"

class Telop : public GameObject
{
public:
	Player* player = nullptr;
	bool ShowDeathTelop = false;
	std::string deathText;
	float offsetX = 40.0f;
	float offsetY = -30.0f;

	Telop();
	~Telop();
	void Update() override;
	void Draw() override;
	void ShowTrap3Message(float duration = 3.0f);
	void StartDeathTelop(Player* p);
	
	//Ž€‚ñ‚¾‚Æ‚«ƒgƒ‰ƒbƒv‚ª‚µ‚á‚×‚é•¶Žš
	bool TouchedTrap1 = false;
	bool TouchedTrap2 = false;
	bool TouchedTrap3 = false;
	bool TouchedTrap4 = false;
	bool TouchedTrap5 = false;
	bool TouchedTrap6 = false;

	float displayTimer = 3.0f;

	

private:
	bool showDeathTelop_ = false;
	float deathTelopX_ = 0.0f;
	float deathTelopY_ = 0.0f;
	float deathTelopTimer_ = 0.0f;

};
