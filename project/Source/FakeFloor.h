#pragma once
#include "../Library/GameObject.h"

class FakeFloor : public GameObject
{
public:
	FakeFloor(int sx, int sy);
	~FakeFloor();

	void Update() override;
	void Draw() override;

	// px,py,pw,ph = プレイヤー等の矩形
	// 触れたら true を返し、FakeFloor は消える
	bool CheckHitBall(float px, float py, float pw, float ph);

	// 必要なら外部で確認用
	bool IsVanished() const { return vanished_; }

private:
	int hImage;
	float x, y;
	static const int width = 64;
	static const int height = 64;

	bool vanished_ = false;
};
