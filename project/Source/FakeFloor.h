#pragma once
#include "../Library/GameObject.h"

class FakeFloor : public GameObject
{
public:
	FakeFloor(int sx, int sy);
	~FakeFloor();

	void Update() override;
	void Draw() override;

	bool CheckHitBall(float px, float py, float pw, float ph);

private:
	int hImage;
	float x, y;
	static const int width = 64;
	static const int height = 64;
};