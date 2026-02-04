#pragma once
#include "../Library/GameObject.h"
#include <vector>

class Boss : public GameObject
{
public:
	Boss(int sx, int sy);
	~Boss();

	void Update() override;
	void Draw() override;

	// 外部（例：プレイヤー弾）からボスにダメージを与えたい時に使う
	void Damage(int amount);

	int  GetHP() const { return hp; }
	bool IsDead() const { return hp <= 0; }

	float GetX() const { return x; }
	float GetY() const { return y; }
	float GetHitR() const { return hitR; }

private:
	// =====================
	// 見た目・基本情報
	// =====================
	int   hImage;
	float x, y;
	int   hp;

	// ボスの当たり判定（円）
	float hitR;

	// =====================
	// 弾幕（敵弾）定義
	// =====================
	struct EnemyBullet
	{
		float x, y;
		float vx, vy;
		float r;
		int   color;
		bool  alive;
	};

	std::vector<EnemyBullet> bullets;

	// 弾幕用タイマー
	int   fireTimer;
	float phaseAngle;

private:
	// 生成・更新・当たり判定
	void UpdateBullets();
	void DrawBullets() const;
	void RemoveDeadBullets();

	void FirePattern_Ring(int n, float speed);
	void FirePattern_FanToPlayer(int n, float speed, float spreadDeg);

	void CheckHitPlayer();      // 敵弾→プレイヤー
	bool CircleHit(float ax, float ay, float ar, float bx, float by, float br) const;

	// UI（デバッグ用）
	void DrawDebug() const;
};