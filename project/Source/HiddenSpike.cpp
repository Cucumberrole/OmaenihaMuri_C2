#include "HiddenSpike.h"
#include "Player.h"
#include "Telop.h"
#include "Collision.h"
#include <DxLib.h>

HiddenSpike::HiddenSpike(float sx, float sy, float triggerRange)
{
	hImage = LoadGraph("data/image/hari.png");

	x = sx;
	y = sy;
	this->triggerRange = triggerRange;

	GetGraphSize(hImage, &width, &height);
}

HiddenSpike::~HiddenSpike()
{
	if (hImage != -1)
	{
		DeleteGraph(hImage);
	}
}

void HiddenSpike::Update()
{
	Player* player = FindGameObject<Player>();
	if (!player) return;

	float cx, cy, cr;
	player->GetHitCircle(cx, cy, cr);

	// まだ見えていない場合：一定距離まで近づいたら「出現」
	if (!revealed)
	{
		float sx = x + width * 0.5f;
		float sy = y + height * 0.5f;

		float dx = cx - sx;
		float dy = cy - sy;
		float dist2 = dx * dx + dy * dy;

		if (dist2 <= triggerRange * triggerRange)
		{
			revealed = true;
		}

		// 見えていない間はダメージなし
		return;
	}

	// ここから先は「出現後」の当たり判定（常にトゲ扱い）
	{
		// 上向き三角形（先端が下）の例
		float sx = x;
		float sy = y;

		VECTOR t1 = VGet(sx, sy, 0.0f);           // 左上
		VECTOR t2 = VGet(sx + width, sy, 0.0f);           // 右上
		VECTOR t3 = VGet(sx + width * 0.5f, sy + height, 0.0f);           // 下の先端

		VECTOR center = VGet(cx, cy, 0.0f);

		if (HitCheck_Circle_Triangle(center, cr, t1, t2, t3))
		{
			player->ForceDie();
			player->SetDead();
		}
	}
}

void HiddenSpike::Draw()
{
	if (!revealed) return; // 見えてない間は描画しない

	DrawGraph(static_cast<int>(x), static_cast<int>(y), hImage, TRUE);
}