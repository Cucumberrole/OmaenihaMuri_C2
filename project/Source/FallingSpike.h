#pragma once
#include "../Library/GameObject.h"

class FallingSpike : public GameObject
{
public:
	// chaseAfterLand が true の針だけ、
	// 落ちて地面に着いたあとプレイヤー方向へ平行移動します
	FallingSpike(int sx, int sy, bool chaseAfterLand = false);
	~FallingSpike();

	void Update() override;
	void Draw() override;

private:
	int   hImage;

	float x, y;
	float vy;
	float gravity;
	int   width;      // 画像幅
	int   height;     // 画像高さ

	bool  landed;     // 地面に着地したか

	// 追加：着地後に追いかけるための情報
	bool  isChaser;      // この針が「追尾針」かどうか
	bool  startedChase;  // 一度でも横移動を開始したか
	float vx;            // 横方向速度
	float chaseSpeed;    // 追尾時の基本スピード
};
