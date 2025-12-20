#pragma once
#include "../Library/GameObject.h"


class Player : public GameObject
{
public:
	Player();
	Player(int sx, int sy);
	~Player();

	void Update() override;
	void Draw() override;

	float GetX() const { return x; }
	float GetY() const { return y; }

	// 絶対座標セット
	//void SetX(float nx) { x = nx; }
	//void SetY(float ny) { y = ny; }

	void SetDead() { isDead = true; }
	bool IsDead() const { return isDead; }
	void ForceDie();

	// 円形当たり判定の取得
	float GetRadius() const { return hitRadius; }
	void GetHitCircle(float& outX, float& outY, float& outRadius) const;

	void PushByWall(float dx);
	// （もしデバッグ用に描画したければ）
	// void DrawHitCircleDebug() const;

private:
	// --- 基本パラメータ ---
	int hImage;
	float x, y;         // 左上座標
	float velocity;
	bool onGround;
	int jumpcount;
	int Maxjumpcount;
	int hp;

	bool isDead;  // ← プレイヤーが死んで固定状態か？

	// 円当たり判定用 (中心からの半径だけ持っておく)
	float hitRadius;

	// --- アニメーション設定 ---
	static const int CHARACTER_WIDTH = 64;
	static const int CHARACTER_HEIGHT = 64;
	static const int ATLAS_WIDTH = 12;
	static const int ATLAS_HEIGHT = 1;
	static const int ANIM_FRAME_COUNT = 12;
	static const int ANIM_FRAME_INTERVAL = 5;
	static const int WALK_SPEED = 3;

	// --- アニメ関連 ---
	int  animIndex;
	int  animFrame;
	bool frip;
};