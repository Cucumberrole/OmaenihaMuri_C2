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
	int hImage;          // 床画像
	float x, y;          // 位置
	float velocityY;     // 落下速度
	bool isFalling;      // 落下中か？
	bool isLanded;       // 着地済みか？
	float gravity;       // 重力加速度

	void StartFalling(); // プレイヤー検知で落下開始
};