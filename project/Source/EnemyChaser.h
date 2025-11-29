#pragma once
#include "../Library/GameObject.h"

class EnemyChaser : public GameObject
{
public:
	EnemyChaser(float sx, float sy);
	~EnemyChaser();

	void Update() override;
	void Draw() override;

private:
	int hImage;
	int bgImage;

	float x, y;
	float speed;

	// アニメーション
	int animIndex;        // 現在のコマ番号（0 or 1）
	int animFrame;        // フレームカウンタ
	static const int CHARACTER_WIDTH = 640;   // 1コマの横幅
	static const int CHARACTER_HEIGHT = 640;  // 1コマの縦幅
	static const int ANIM_FRAME_INTERVAL = 4; // コマ切替速度
};