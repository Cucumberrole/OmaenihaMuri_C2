#pragma once
#include "../Library/GameObject.h"

class Telop : public GameObject
{
public:
	Telop();
	~Telop();
	void Update() override;
	void Draw() override;
	bool TouchedTrap1 = false;
	bool TouchedTrap2 = false;
	bool TouchedTrap3 = false;
	bool TouchedTrap4 = false;
	bool TouchedTrap5 = false;
	bool TouchedTrap6 = false;
	bool TouchedTrap7 = false;
	bool TouchedTrap8 = false;
	bool TouchedTrap9 = false;
	bool TouchedTrap10 = false;

private:
	
};

