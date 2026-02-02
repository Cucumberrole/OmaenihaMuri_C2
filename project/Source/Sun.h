#pragma once
#include "../Library/GameObject.h"
#include <vector>

class Sun :public GameObject
{
public:
	Sun(float sx,float sy);
	~Sun();
	void Update()override;
	void Draw()override;
private:
	int hImage;
	float x, y;        // 左上座標
	float vx;          // 横方向の速度
	float vy;          // 落下速度

	float moveTimer;
	float changeInterval;

	int   size;        // 表示上の直径（例：64）

	int SunSE;
};

