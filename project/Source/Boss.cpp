#include "Boss.h"
#include "Player.h"
#include <algorithm>
#include <cmath>
#include <DxLib.h>

static inline float Deg2Rad(float deg) { return deg * 3.1415926535f / 180.0f; }

Boss::Boss(int sx, int sy)
{
	hImage = LoadGraph("data/image/sui2.png");
	bulletImage = LoadGraph("data/image/sui1.png");

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
		// phaseAngle を増やすほど、スパイラル系の弾が回転して「弾幕の表情」が変わります
		// ここを変えると、全体の回転速度が変わります（例：2.0f→4.0fで倍速）
		phaseAngle += Deg2Rad(2.0f); // 回転速度（弾幕の表情が変わる）

		// ===== 弾幕フェーズ（状態）切り替え =====
		// fireTimer を 400 で割った商を 0,1,2... と増やし、%3 で 0/1/2 を繰り返しています。
		// つまり「400フレームごとに弾幕状態が変わる」設計です。
		const int phase = (fireTimer / 400) % 3;
		// ===== フェーズごとの弾幕内容（ここをいじると弾幕の状態が変わります）=====
		// ・撃つ間隔（% 12, % 90 など）
		// ・呼ぶパターン関数（AimAtPlayer / Ring / Spiral...）
		// ・各パターンの引数（弾数n, 速度speed, 広がりspreadDeg など）
		// を変えると弾幕が変化します。
		if (phase == 0)
		{
			// 基本：自機狙い＋時々リング
			if (fireTimer % 12 == 0) FirePattern_AimAtPlayer(5.0f);
			if (fireTimer % 60 == 0) FirePattern_Ring(24, 3.0f);
		}
		else if (phase == 1)
		{
			// スパイラル：毎フレーム薄く吐く（n小さめ推奨）
			if (fireTimer % 2 == 0)  FirePattern_Spiral(2, 3.2f, Deg2Rad(22.0f));
			if (fireTimer % 60 == 0) FirePattern_FanToPlayer(9, 4.0f, 60.0f);
			if (fireTimer % 90 == 0) FirePattern_Ring(24, 3.0f);
		}
		else
		{
			// デュアルスパイラル＋ウェーブリング
			if (fireTimer % 3 == 0)  FirePattern_SpiralDual(1, 3.0f, Deg2Rad(30.0f));
			if (fireTimer % 45 == 0) FirePattern_WaveRing(28, 2.4f, 1.2f, 120.0f);
			if (fireTimer % 60 == 0) FirePattern_Ring(24, 3.0f);
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

// 円同士の当たり判定（距離^2 <= (半径和)^2）
bool Boss::CircleHit(float ax, float ay, float ar, float bx, float by, float br) const
{
	float dx = ax - bx;
	float dy = ay - by;
	float rr = ar + br;
	return (dx * dx + dy * dy) <= (rr * rr);
}

void Boss::CheckHitPlayer()
{
	// ===== プレイヤーの当たり判定（敵弾→プレイヤー） =====
	Player* player = FindGameObject<Player>();
	if (!player) return;

	// ★当たり判定は Player 側に集約（変更すると判定もデバッグ表示も自動で揃う）
	float px, py, pr;
	player->GetBulletHitCircle(px, py, pr);

	for (auto& b : bullets)
	{
		if (!b.alive) continue;

		if (CircleHit(b.x, b.y, b.r, px, py, pr))
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
	// ===== デバッグ表示：当たり判定の可視化 =====
	// ・ボスの当たり判定（円）
	// ・プレイヤーの当たり判定（弾幕用の小さい円）
	//
	// いつでも表示したくない場合は、ここをキー入力でON/OFFするようにしてください。

	// プレイヤー当たり判定（弾幕用の小さい当たり中心を「点」で表示）
	{
		Player* player = FindGameObject<Player>();
		if (player)
		{
			// ※当たり判定の中心は「プレイヤー画像(64x64)の中央」を想定
			//   すでに中心修正済みなら、この計算に合わせてください（GetX/GetYが左上なら +32,+32）
			float px, py, pr;
			player->GetBulletHitCircle(px, py, pr);

			const int cx = (int)px;
			const int cy = (int)py;
			const int col = GetColor(0, 0, 0);

			// 1ピクセルだと見えにくいので「点（3x3）」として描画
			DrawBox(cx - 1, cy - 1, cx + 2, cy + 2, col, TRUE);
			// さらに分かりやすくするなら十字を有効化（不要なら消してください）
			DrawLine(cx - 6, cy, cx + 6, cy, col);
			DrawLine(cx, cy - 6, cx, cy + 6, col);
		}
	}
}
