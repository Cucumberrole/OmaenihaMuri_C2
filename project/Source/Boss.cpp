#include "Boss.h"
#include "Player.h"
#include <algorithm>
#include <cmath>
#include <DxLib.h>

static inline float Deg2Rad(float deg) { return deg * 3.1415926535f / 180.0f; }

Boss::Boss(int sx, int sy)
{
	hImage = LoadGraph("data/image/sui2.png");
	bulletImage = LoadGraph("data/image/sui1.png"); // ←自機の画像ファイル名に変更

	x = (float)sx;
	y = (float)sy;

	baseX = x;
	baseY = y;
	moveAngle = 0.0f;

	// 初期は停止→最初の移動先を決める
	moveState = MoveState::Wait;
	waitTimer = waitDuration;

	// どっちへ行くか（最初は右端に向かう例）
	moveTargetX = x;
	vx = 0.0f;

	hp = 300;
	hitR = 40.0f;

	fireTimer = 0;
	phaseAngle = 0.0f;

	bullets.reserve(2048);

	SetDrawOrder(5);
}

Boss::~Boss()
{
	DeleteGraph(hImage);
}

void Boss::Damage(int amount)
{
	if (hp <= 0) return;
	hp -= amount;
	if (hp < 0) hp = 0;
}

void Boss::Update()
{
	// 時間経過でクリア
	if (!timeUpClear)
	{
		surviveFrame++;
		if (surviveFrame >= clearFrame)
		{
			timeUpClear = true;

			// クリア時に弾を消す（見た目も気持ちいい）
			bullets.clear();
		}
	}

	// クリアしたら撃たない・動かない等（任意）
	if (timeUpClear)
	{
		// クリア演出だけしたいならここでreturnしてOK
		return;
	}




	// ===== Boss movement: wall-to-wall + pause =====
	int sw = 1280, sh = 720;
	GetDrawScreenSize(&sw, &sh);

	const float leftX = wallMarginX;
	const float rightX = (float)sw - wallMarginX;

	auto StartMoveTo = [&](float tx)
		{
			moveTargetX = tx;
			const float dir = (moveTargetX > x) ? 1.0f : -1.0f;
			vx = dir * moveSpeedX;
			moveState = MoveState::Move;
		};

	if (moveState == MoveState::Wait)
	{
		waitTimer--;
		if (waitTimer <= 0)
		{
			// 次の移動先：今いる側と反対端へ
			float next = (x < (leftX + rightX) * 0.5f) ? rightX : leftX;
			StartMoveTo(next);
		}
	}
	else // Move
	{
		x += vx;

		// 目標到達（行き過ぎ防止込み）
		if ((vx > 0.0f && x >= moveTargetX) || (vx < 0.0f && x <= moveTargetX))
		{
			x = moveTargetX;
			vx = 0.0f;

			moveState = MoveState::Wait;
			waitTimer = waitDuration;
		}
	}

	// 安全のためクランプ（端を絶対超えない）
	if (x < leftX) x = leftX + 128;
	if (x > rightX) x = rightX + 128;










	// Boss stays still

	if (hp > 0)
	{
		fireTimer++;
		phaseAngle += Deg2Rad(2.0f); // 回転速度（弾幕の表情が変わる）

		// フェーズ切り替え（例：300フレーム周期）
		const int phase = (fireTimer / 400) % 3;

		if (phase == 0)
		{
			// 基本：自機狙い＋時々リング
			if (fireTimer % 12 == 0) FirePattern_AimAtPlayer(5.0f);
			if (fireTimer % 90 == 0) FirePattern_Ring(24, 3.0f);
		}
		else if (phase == 1)
		{
			// スパイラル：毎フレーム薄く吐く（n小さめ推奨）
			if (fireTimer % 2 == 0)  FirePattern_Spiral(2, 3.2f, Deg2Rad(22.0f));
			if (fireTimer % 60 == 0) FirePattern_FanToPlayer(9, 4.0f, 60.0f);
		}
		else
		{
			// デュアルスパイラル＋ウェーブリング
			if (fireTimer % 3 == 0)  FirePattern_SpiralDual(1, 3.0f, Deg2Rad(30.0f));
			if (fireTimer % 45 == 0) FirePattern_WaveRing(28, 2.4f, 1.2f, 120.0f);
		}
	}
	else
	{
		// hp <= 0 なら通常は撃たない（必要なら演出用に残してもOK）
		fireTimer++;
	}

	UpdateBullets();
	CheckHitPlayer();
	RemoveDeadBullets();
}

void Boss::UpdateBullets()
{
	int sw = 1280, sh = 720;
	GetDrawScreenSize(&sw, &sh);

	for (auto& b : bullets)
	{
		if (!b.alive) continue;

		b.x += b.vx;
		b.y += b.vy;

		const float margin = 200.0f;
		if (b.x < -margin || b.x > sw + margin || b.y < -margin || b.y > sh + margin)
		{
			b.alive = false;
		}
	}
}

void Boss::RemoveDeadBullets()
{
	bullets.erase(
		std::remove_if(bullets.begin(), bullets.end(),
			[](const EnemyBullet& b) { return !b.alive; }),
		bullets.end()
	);
}

void Boss::FirePattern_Ring(int n, float speed)
{
	for (int i = 0; i < n; i++)
	{
		float a = (2.0f * 3.1415926535f) * (float)i / (float)n;

		EnemyBullet b{};
		b.x = x; b.y = y;
		b.vx = std::cos(a) * speed;
		b.vy = std::sin(a) * speed;
		b.r = 6.0f;
		b.color = GetColor(255, 220, 80);
		b.alive = true;

		bullets.push_back(b);
	}
}

void Boss::FirePattern_AimAtPlayer(float speed)
{
	Player* player = FindGameObject<Player>();
	if (!player) return;

	// If your Player origin is top-left, adding half-size makes aiming feel natural.
	const float px = (float)player->GetX() + 32.0f;
	const float py = (float)player->GetY() + 32.0f;

	const float ang = std::atan2(py - y, px - x);

	EnemyBullet b{};
	b.x = x; b.y = y;
	b.vx = std::cos(ang) * speed;
	b.vy = std::sin(ang) * speed;
	b.r = 5.0f;
	b.color = GetColor(120, 220, 255);
	b.alive = true;

	bullets.push_back(b);
}

void Boss::FirePattern_Spiral(int n, float speed, float angStepRad)
{
	// phaseAngle を基準に、毎回少しずつ角度を進めてスパイラルにする
	for (int i = 0; i < n; i++)
	{
		const float a = phaseAngle + angStepRad * (float)i;

		EnemyBullet b{};
		b.x = x; b.y = y;
		b.vx = cosf(a) * speed;
		b.vy = sinf(a) * speed;
		b.r = 5.0f;
		b.color = GetColor(255, 180, 80);
		b.alive = true;
		bullets.push_back(b);
	}
}

void Boss::FirePattern_SpiralDual(int n, float speed, float angStepRad)
{
	// 逆回転ペアで“弾幕感”が一気に上がる
	for (int i = 0; i < n; i++)
	{
		const float a1 = phaseAngle + angStepRad * (float)i;
		const float a2 = -phaseAngle - angStepRad * (float)i;

		EnemyBullet b1{};
		b1.x = x; b1.y = y;
		b1.vx = cosf(a1) * speed;
		b1.vy = sinf(a1) * speed;
		b1.r = 5.0f;
		b1.color = GetColor(255, 120, 255);
		b1.alive = true;
		bullets.push_back(b1);

		EnemyBullet b2{};
		b2.x = x; b2.y = y;
		b2.vx = cosf(a2) * speed;
		b2.vy = sinf(a2) * speed;
		b2.r = 5.0f;
		b2.color = GetColor(120, 255, 255);
		b2.alive = true;
		bullets.push_back(b2);
	}
}

void Boss::FirePattern_WaveRing(int n, float baseSpeed, float waveAmp, float wavePeriodFrames)
{
	// リングの速度が周期的に変わる（密度が変わって“弾幕っぽい”）
	const float t = (wavePeriodFrames <= 0.0f) ? 0.0f : (float)fireTimer / wavePeriodFrames;
	const float s = baseSpeed + sinf(t * 2.0f * 3.1415926535f) * waveAmp;

	for (int i = 0; i < n; i++)
	{
		const float a = (2.0f * 3.1415926535f) * (float)i / (float)n + phaseAngle * 0.25f;

		EnemyBullet b{};
		b.x = x; b.y = y;
		b.vx = cosf(a) * s;
		b.vy = sinf(a) * s;
		b.r = 5.0f;
		b.color = GetColor(255, 255, 120);
		b.alive = true;
		bullets.push_back(b);
	}
}

void Boss::FirePattern_FanToPlayer(int n, float speed, float spreadDeg)
{
	Player* player = FindGameObject<Player>();
	if (!player) return;

	const float px = (float)player->GetX() + 32.0f;
	const float py = (float)player->GetY() + 32.0f;

	const float base = std::atan2(py - y, px - x);
	const float spread = Deg2Rad(spreadDeg);

	for (int i = 0; i < n; i++)
	{
		const float t = (n == 1) ? 0.0f : (float)i / (float)(n - 1);
		const float ang = base - spread * 0.5f + spread * t + phaseAngle;

		EnemyBullet b{};
		b.x = x; b.y = y;
		b.vx = std::cos(ang) * speed;
		b.vy = std::sin(ang) * speed;
		b.r = 12.0f;
		b.color = GetColor(255, 160, 255);
		b.alive = true;

		bullets.push_back(b);
	}
}

bool Boss::CircleHit(float ax, float ay, float ar, float bx, float by, float br) const
{
	float dx = ax - bx;
	float dy = ay - by;
	float rr = ar + br;
	return (dx * dx + dy * dy) <= (rr * rr);
}

void Boss::CheckHitPlayer()
{
	Player* player = FindGameObject<Player>();
	if (!player) return;

	const float playerR = 10.0f;
	float px = (float)player->GetX();
	float py = (float)player->GetY();

	for (auto& b : bullets)
	{
		if (!b.alive) continue;

		if (CircleHit(b.x, b.y, b.r, px, py, playerR))
		{
			b.alive = false;
			player->ForceDie();
			return;
		}
	}
}

void Boss::Draw()
{
	if (hImage >= 0)
	{
		DrawExtendGraph((int)(x - 128), (int)(y - 128), (int)(x + 128), (int)(y + 128), hImage, TRUE);
	}
	else
	{
		DrawCircle((int)x, (int)y, (int)hitR, GetColor(255, 120, 120), TRUE);
	}

	// HP bar (example)
	//{
	//	const int maxHp = 300;
	//	const int barW = 400;
	//	const int barH = 18;
	//	const int x0 = 20;
	//	const int y0 = 20;

	//	int cur = hp;
	//	if (cur < 0) cur = 0;
	//	int fillW = (barW * cur) / maxHp;

	//	DrawBox(x0, y0, x0 + barW, y0 + barH, GetColor(0, 0, 0), TRUE);
	//	DrawBox(x0 + 2, y0 + 2, x0 + 2 + fillW, y0 + barH - 2, GetColor(255, 80, 80), TRUE);
	//	DrawBox(x0, y0, x0 + barW, y0 + barH, GetColor(255, 255, 255), FALSE);
	//}

	DrawBullets();
	DrawDebug();
}

void Boss::DrawBullets() const
{
	for (const auto& b : bullets)
	{
		if (!b.alive) continue;

		const int half = 16; // 32x32 の半分
		DrawExtendGraph((int)(b.x - half), (int)(b.y - half),
			(int)(b.x + half), (int)(b.y + half),
			bulletImage, TRUE);
	}
}


void Boss::DrawDebug() const
{
	//SetFontSize(24);
	//int h = GetFontSize();

	//int remain = (clearFrame - surviveFrame);
	//if (remain < 0) remain = 0;

	//DrawFormatString(20, 50, GetColor(255, 255, 255),
	//	"TIME: %d", remain / 60);

	//DrawFormatString(0, 200 + h * 0, GetColor(255, 255, 255), "BossX: %.2f", x);
	//DrawFormatString(0, 200 + h * 1, GetColor(255, 255, 255), "BossY: %.2f", y);
	//DrawFormatString(0, 200 + h * 2, GetColor(255, 255, 255), "BossHP: %d", hp);
	//DrawFormatString(0, 200 + h * 3, GetColor(200, 200, 200), "EnemyBullets: %d", (int)bullets.size());
	//DrawFormatString(0, 200 + h * 4, GetColor(200, 200, 200), "fireTimer: %d", fireTimer);

}
