#pragma once
#include "../Library/GameObject.h"

// 触れたら消える床
class FakeFloor : public GameObject
{
public:
	FakeFloor(int sx, int sy);
	~FakeFloor();

	void Update() override;
	void Draw() override;

	// px,py,pw,ph = プレイヤーの矩形（左上座標＋幅高さ）
	// 触れたら true を返し、この床は消えます
	bool CheckHitBall(float px, float py, float pw, float ph);

	// 消えたかどうか
	bool IsVanished() const { return vanished_; }

private:
	int hImage = -1;
	float x = 0.0f, y = 0.0f;

	static const int width = 64;
	static const int height = 64;

	bool vanished_ = false;
};
