#pragma once
#include "../Library/GameObject.h"

class Dokan2 : public GameObject
{
public:
	Dokan2(int sx, int sy);
	~Dokan2();
	void Update() override;
	void Draw() override;

private:
	int dImage;
	int x, y;

};