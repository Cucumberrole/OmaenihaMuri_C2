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

	void Damage(int amount);

	int  GetHP() const { return hp; }
	bool IsDead() const { return hp <= 0; }

	float GetX() const { return x; }
	float GetY() const { return y; }
	float GetHitR() const { return hitR; }

	bool IsTimeUpClear() const { return timeUpClear; }

private:
	int   hImage;
	float x, y;
	int   hp;
	float hitR;

	// =====================
	// Enemy bullets
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

	// ===== 弾幕（攻撃パターン）制御 =====
	// fireTimer : フレームカウンタ（弾を撃つ周期・フェーズ切替に使用）
	// phaseAngle: スパイラル等の回転基準角（毎フレーム増やして回転させる）
	int   fireTimer = 0;
	float phaseAngle = 0.0f;

private:
	void UpdateBullets();
	void DrawBullets() const;
	void RemoveDeadBullets();

	// =====================
	// 攻撃パターン（弾幕）
	// ここで「どんな弾を生成するか」を定義しています。
	// Update() の phase 分岐から呼ばれます。
	// =====================
	// Patterns
	void FirePattern_Ring(int n, float speed);
	void FirePattern_AimAtPlayer(float speed);              // <- single aimed shot
	void FirePattern_FanToPlayer(int n, float speed, float spreadDeg);
	void FirePattern_Spiral(int n, float speed, float angStepRad);
	void FirePattern_SpiralDual(int n, float speed, float angStepRad);
	void FirePattern_WaveRing(int n, float baseSpeed, float waveAmp, float wavePeriodFrames);

	void CheckHitPlayer();
	bool CircleHit(float ax, float ay, float ar, float bx, float by, float br) const;

	void DrawDebug() const;

	// ===== Boss movement (simple horizontal patrol) =====
	float baseX = 0.0f;
	float baseY = 0.0f;
	float moveAngle = 0.0f;     // 角度（サイン波用）
	float moveSpeed = 0.035f;   // 速さ（小さいほどゆっくり）
	float moveAmpX = 220.0f;    // 左右の振れ幅
	float moveAmpY = 0.0f;      // 上下も動かすなら使う

	// ===== Boss movement (wall-to-wall + pause) =====
	enum class MoveState { Move, Wait };

	MoveState moveState = MoveState::Wait;

	float vx = 0.0f;              // x方向速度（Move中だけ使う）
	float moveTargetX = 0.0f;     // 目標x

	int waitTimer = 0;            // 停止中カウンタ
	int waitDuration = 60;        // 停止時間（フレーム）例：60=1秒(60FPS)

	float moveSpeedX = 6.0f;      // 移動速度（px/frame）
	float wallMarginX = 140.0f;   // 端の余白（ボス半サイズより少し大きめ）


private:
	int bulletImage; // ← 追加（敵弾の見た目に自機画像を使う）

private:
	int  surviveFrame = 0;     // 経過フレーム
	int  clearFrame = 60 * 50; // 例：30秒生き残りでクリア（60FPS想定）
	bool timeUpClear = false;
};
