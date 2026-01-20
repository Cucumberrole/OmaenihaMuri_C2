#pragma once
#include "../Library/GameObject.h"

class MovingSmallTrap :public GameObject
{
public:
	MovingSmallTrap(float sx, float sy);
	~MovingSmallTrap();
	void Update() override;
	void Draw()override;
private:
	float speed;
	bool moved = false;
	float x = 0.0f;
	float y = 0.0f;
	int   width = 16;
	int   height = 16;
	int SImage = -1;
	const float tile = 64.0f;
};

