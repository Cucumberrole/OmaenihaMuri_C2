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
	float x, y;        // 左上座標
	float vx;          // 横方向速度
	float vy;          // 落下速度
	float gravity;     // 重力

	int size;          // ボールの直径（画像サイズ）

	void Bounce();     // 壁反転
};