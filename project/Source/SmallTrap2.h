#pragma once
#include "../Library/GameObject.h"

class SmallTrap2 : public GameObject
{
public:
	SmallTrap2(int x, int y);
	~SmallTrap2();
	void Update() override;
	void Draw() override;
private:
	int SImage;    // ¬‚³‚¢j‚Ì‰æ‘œƒnƒ“ƒhƒ‹
	float x = 0.0f;
	float y = 0.0f;
	int   width = 16;
	int   height = 16;
};

