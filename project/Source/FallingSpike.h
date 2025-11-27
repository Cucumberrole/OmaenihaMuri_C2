#pragma once
#include "../Library/GameObject.h"

class FallingSpike : public GameObject
{
public:
	FallingSpike(int sx, int sy);
	~FallingSpike();

	void Update() override;
	void Draw() override;

private:
	int hImage;

	float x, y;
	float vy;
	float gravity;

	bool landed;
};