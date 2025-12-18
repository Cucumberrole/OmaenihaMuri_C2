#pragma once
#include "../Library/GameObject.h"

class FallingFloor : public GameObject
{
public:
	FallingFloor(int sx, int sy);
	~FallingFloor();

	void Update() override;
	void Draw() override;

	bool IsLanded() const { return isLanded; }
	float GetX() const { return x; }
	float GetY() const { return y; }

	int HitCheckRight(int px, int py);
	int HitCheckLeft(int px, int py);
	int HitCheckDown(int px, int py);
	int HitCheckUp(int px, int py);

private:
	int hImage;
	float x, y;

	float velocityY;
	bool  isFalling;
	bool  isLanded;
	float gravity;

	void StartFalling();
};