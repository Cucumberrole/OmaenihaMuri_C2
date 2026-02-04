#pragma once
#include "../Library/GameObject.h"

class PlayerBullet : public GameObject
{
public:
	PlayerBullet(float sx, float sy, float vx, float vy);
	~PlayerBullet();

	void Update() override;
	void Draw() override;

private:
	float x = 0.0f;
	float y = 0.0f;
	float vx = 0.0f;
	float vy = 0.0f;

	float r = 6.0f;
	int color = 0;
	bool alive = true;
};