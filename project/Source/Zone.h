#pragma once
#include "../Library/GameObject.h"

class Zone :public GameObject
{
public:
	Zone(float x, float y, float w = 64.0f, float h = 64.0f);
	~Zone();
	void Update()override;
	void Draw()override;
private:
	float x, y;
	float width, height;
};

