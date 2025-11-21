#pragma once
#include "../Library/GameObject.h"

class Dokan : public GameObject
{
public:
	Dokan(int sx,int sy);
	~Dokan();
	void Update() override;
	void Draw() override;

private:
	int dImage;
	int x, y;

};