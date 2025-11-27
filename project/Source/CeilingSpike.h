#pragma once
#include "../Library/GameObject.h"

class CeilingSpike : public GameObject
{
public:
	CeilingSpike(int sx, int sy);
	~CeilingSpike();

	void Update() override;
	void Draw() override;

private:
	int hImage;
	float x, y;
	bool triggered;

	void SpawnFallingSpike();
};