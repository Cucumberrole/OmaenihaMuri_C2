#pragma once
#include "../Library/GameObject.h"

class PatrolEnemy : public GameObject
{
public:
	PatrolEnemy(float sx, float sy, float speed = 1.2f);
	~PatrolEnemy();

	void Update() override;
	void Draw() override;

private:
	int   hImage = -1;

	float x = 0.0f;
	float y = 0.0f;

	float vx = 0.0f;
	float speed = 1.2f;

	int   w = 64;
	int   h = 64;

	// collision helper
	bool  HitPlayer() const;
	bool  IsSolidAt(float px, float py) const;
};
