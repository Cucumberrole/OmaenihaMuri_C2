#pragma once
#include "../Library/GameObject.h"

class MovingWall : public GameObject
{
public:
	// baseX, baseY : 一番下のブロックの左上座標
	// dir          : -1 = 左へ動く, +1 = 右へ動く
	MovingWall(float baseX, float baseY, int dir);
	~MovingWall();

	void Update() override;
	void Draw() override;

	// プレイヤーのポイント当たり判定用（Player.cpp から呼ぶ用）
	int HitCheckRight(int px, int py);
	int HitCheckLeft(int px, int py);
	int HitCheckDown(int px, int py);
	int HitCheckUp(int px, int py);

private:
	enum class State
	{
		BUILDING,
		MOVING,
		STOPPED
	};

	int   hImage;
	float x, y;
	int   width;
	int   blockSize;
	int   buildLevel;
	int   maxLevel;
	int   buildTimer;
	int   buildInterval;

	float moveSpeed;
	int   dir;
	State state;

	// このフレームで壁が実際に動いた量（壁に当たって止まったら 0）
	float lastMoveDx;

	void BuildStep();
	void MoveStep();
	void CheckCrushWithPlayer();
};