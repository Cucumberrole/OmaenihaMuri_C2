#pragma once
#include "../Library/GameObject.h"

class FakeFloor : public GameObject
{
public:
	FakeFloor(int sx, int sy);
	~FakeFloor();

	void Update() override;
	void Draw() override;

private:
	int hImage;
	float x, y;
};