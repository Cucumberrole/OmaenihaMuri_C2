#include "Boss.h"
#include "Player.h"
#include <algorithm>
#include <cmath>
#include <DxLib.h>

// 変更前：DegToRad
static inline float Deg2Rad(float deg) { return deg * 3.1415926535f / 180.0f; }

Boss::Boss(int sx, int sy)
{
	// 画像は任意。なければ DrawCircle に置き換えてもOK
	hImage = LoadGraph("data/image/すい2.png");

	x = (float)sx;
	y = (float)sy;

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
	// 固定ボスなので移動処理なし

	// 弾幕生成（ここを増やすと「フェーズ制」に発展できます）
	if (hp > 0)
	{
		fireTimer++;
		phaseAngle += Deg2Rad(1.0f);

		// 例：定期的に全方位リング
		if (fireTimer % 90 == 0)
		{
			FirePattern_Ring(24, 3.2f);
		}

		// 例：高頻度で自機狙い扇状
		if (fireTimer % 18 == 0)
		{
			FirePattern_FanToPlayer(9, 4.0f, 60.0f);
		}
	}

	UpdateBullets();
	CheckHitPlayer();
	RemoveDeadBullets();
}

void Boss::UpdateBullets()
{
	for (auto& b : bullets)
	{
		if (!b.alive) continue;

		b.x += b.vx;
		b.y += b.vy;

		// 画面外で消す（値は適当に余裕）
		if (b.x < -100 || b.x > 1280 + 100 || b.y < -100 || b.y > 720 + 100)
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
	// 全方位リング弾
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

void Boss::FirePattern_FanToPlayer(int n, float speed, float spreadDeg)
{
	Player* player = FindGameObject<Player>();
	if (!player) return;

	// プレイヤー中心を狙う（見た目が自然）
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
		b.r = 5.0f;
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

	// プレイヤーの当たり半径：あなた側の設計が不明なので、仮に小さめ固定
	// （もし Player に GetHitR() みたいなのがあるなら置き換えてください）
	const float playerR = 10.0f;
	float px = (float)player->GetX();
	float py = (float)player->GetY();

	for (auto& b : bullets)
	{
		if (!b.alive) continue;

		if (CircleHit(b.x, b.y, b.r, px, py, playerR))
		{
			b.alive = false;

			// あなたの既存コードに合わせて「即死」処理にしています（必要なら差し替え）
			player->ForceDie();
			return;
		}
	}
}

void Boss::Draw()
{
	// ボス描画
	if (hImage >= 0)
	{
		DrawExtendGraph((int)(x - 128), (int)(y - 128), (int)(x + 128), (int)(y + 128), hImage, TRUE);
	}
	else
	{
		DrawCircle((int)x, (int)y, (int)hitR, GetColor(255, 120, 120), TRUE);
	}

	// Boss::Draw() の最後あたりに追加
	{
		const int maxHp = 300; // hp初期値と合わせる
		const int barW = 400;
		const int barH = 18;
		const int x0 = 20;
		const int y0 = 20;

		int cur = hp;
		if (cur < 0) cur = 0;
		int fillW = (barW * cur) / maxHp;

		DrawBox(x0, y0, x0 + barW, y0 + barH, GetColor(0, 0, 0), TRUE);
		DrawBox(x0 + 2, y0 + 2, x0 + 2 + fillW, y0 + barH - 2, GetColor(255, 80, 80), TRUE);
		DrawBox(x0, y0, x0 + barW, y0 + barH, GetColor(255, 255, 255), FALSE);
	}

	// 弾描画
	DrawBullets();

	// デバッグ表示
	DrawDebug();
}

void Boss::DrawBullets() const
{
	for (const auto& b : bullets)
	{
		if (!b.alive) continue;
		DrawCircle((int)b.x, (int)b.y, (int)b.r, b.color, TRUE);
	}
}

void Boss::DrawDebug() const
{
	SetFontSize(24);
	int h = GetFontSize();

	DrawFormatString(0, 200 + h * 0, GetColor(255, 255, 255), "BossX: %.2f", x);
	DrawFormatString(0, 200 + h * 1, GetColor(255, 255, 255), "BossY: %.2f", y);
	DrawFormatString(0, 200 + h * 2, GetColor(255, 255, 255), "BossHP: %d", hp);
	DrawFormatString(0, 200 + h * 3, GetColor(200, 200, 200), "EnemyBullets: %d", (int)bullets.size());
}
