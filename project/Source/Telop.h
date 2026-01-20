#pragma once
#include "../Library/GameObject.h"

class Telop : public GameObject
{
public:
	Telop();
	~Telop();
	void Update() override;
	void Draw() override;
	void ShowTrap3Message(float duration = 3.0f);
	bool TouchedTrap1 = false;
	bool TouchedTrap2 = false;
	bool TouchedTrap3 = false;
	bool TouchedTrap4 = false;
	bool TouchedTrap5 = false;
	float displayTimer = 0.0f;
private:
	
};

