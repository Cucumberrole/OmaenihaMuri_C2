#include "PlayerBullet.h"
#include "Boss.h"
#include <DxLib.h>
#include <cmath>

static inline bool CircleHit(float ax, float ay, float ar, float bx, float by, float br)
{
	const float dx = ax - bx;
	const float dy = ay - by;
	const float rr = ar + br;
	return (dx * dx + dy * dy) <= (rr * rr);
}

PlayerBullet::PlayerBullet(float sx, float sy, float vx_, float vy_)
{
	x = sx;
	y = sy;
	vx = vx_;
	vy = vy_;
	color = GetColor(120, 220, 255);

	SetDrawOrder(6); // ボス弾より上に出したければ調整
}

PlayerBullet::~PlayerBullet()
{
}

void PlayerBullet::Update()
{
	if (!alive) { DestroyMe(); return; }

	x += vx;
	y += vy;

	// 画面外で消す
	int sw = 0, sh = 0;
	GetDrawScreenSize(&sw, &sh);
	if (x < -100 || x > sw + 100 || y < -100 || y > sh + 100)
	{
		alive = false;
		DestroyMe();
		return;
	}

	// ボスに当たったらダメージ
	Boss* boss = FindGameObject<Boss>();
	if (boss && !boss->IsDead())
	{
		// Boss側に座標取得がないので、次項で Getter を追加します
		const float bx = boss->GetX();
		const float by = boss->GetY();
		const float br = boss->GetHitR();

		if (CircleHit(x, y, r, bx, by, br))
		{
			boss->Damage(10);
			alive = false;
			DestroyMe();
			return;
		}
	}
}

void PlayerBullet::Draw()
{
	if (!alive) return;
	DrawCircle((int)x, (int)y, (int)r, color, TRUE);
}