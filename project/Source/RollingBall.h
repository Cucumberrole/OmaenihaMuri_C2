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
	int   hImage;
	float x, y;        // 左上座標
	float vx;          // 横方向の速度（正：右 / 負：左）
	float vy;          // 落下速度
	float gravity;     // 重力

	int   size;        // 表示上の直径（例：64）

	// --- 回転用 ---
	float angle;       // 現在の回転角（ラジアン）
	float scale;       // 500px → 64px に縮小する倍率

	void Bounce();     // 壁に当たった時の反転
};