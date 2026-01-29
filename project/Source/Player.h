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

	float GetW() const { return 64.0f; }
	float GetH() const { return 64.0f; }


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
	bool IsdeathAnimEnd() const;

	bool deathTelopStarted = false;

	//逆操作
	void SetReverse(bool flag) { IsReverse = flag; }

private:
	// --- 基本パラメータ ---
	int hImage;
	float x, y;         // 左上座標
	float velocity;
	bool onGround;
	int jumpcount;
	int Maxjumpcount;
	int hp;
	float pushX;

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

	// ---死亡演出 ---
	enum class DeathState
	{
		None,
		Up,     // 上に飛ぶ
		Fall    // 落下
	};

	DeathState deathState;
	int hDeadUpImage;    // 左の画像
	int hDeadFallImage;  // 右の画像

	bool deathAnimEnd;

	// --- Hit Overlay ---
	static void InitHitOverlay();
	static void TriggerHitOverlay();
	static void UpdateHitOverlay();
	static void DrawHitOverlay();

	static int sHitOverlayGraph;
	static int sHitOverlayTimer;
	static constexpr int HIT_OVERLAY_MAX = 30;

	//逆操作
	bool IsReverse = false;
};