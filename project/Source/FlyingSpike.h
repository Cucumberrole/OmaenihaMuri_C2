#pragma once
#include "../Library/GameObject.h"

class FlyingSpike : public GameObject
{
public:
	FlyingSpike(float startX, float startY, float speed);
	~FlyingSpike();

	void Update() override;
	void Draw() override;

private:
	int hImage;     // 針画像
	float x, y;     // 位置
	float speed;    // 移動速度（正：右、負：左）
	int width;      // 画像幅
	int height;     // 画像高さ
};