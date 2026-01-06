#pragma once
#include "../Library/GameObject.h"

class Telop : public GameObject
{
public:
	Telop();
	~Telop();
	void Update() override;
	void Draw() override;
	bool g_isPlayerTouchedTrap = false;
private:
	int a, b, c, d, e, f, g;
};

