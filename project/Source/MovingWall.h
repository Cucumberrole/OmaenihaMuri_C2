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
		BUILDING,   // 積み上げ中
		MOVING,     // 横移動中
		STOPPED     // 止まった
	};

	int   hImage;        // ブロック画像
	float x, y;          // 一番下のブロックの左上座標
	int   width;         // 横幅（64）
	int   blockSize;     // 1マス 64
	int   buildLevel;    // 今何段目まで出ているか 0〜3
	int   maxLevel;      // 3 段
	int   buildTimer;    // 次の段を出すまでのカウンタ
	int   buildInterval; // 段を追加する間隔(フレーム)

	float moveSpeed;     // 横移動スピード
	int   dir;           // -1 or +1
	State state;

	void BuildStep();          // 1 段積み上げる
	void MoveStep();           // 横移動（壁 vs Field）
	void CheckCrushWithPlayer(); // 毎フレーム押し潰しチェック
};