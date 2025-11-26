#pragma once
#include "../Library/GameObject.h"

class RollingBall : public GameObject
{
public:
	RollingBall(float sx, float sy, float dir);
	~RollingBall();

	void Update() override;
	void Draw() override;

private:
	int hImage;
	float x, y;
	float vx;        // 横方向の速度（正：右 / 負：左）
	float vy;        // 落下速度
	float gravity;   // 重力

	int size;        // 鉄球の直径

	void Bounce();   // 壁に当たった時の反転
};