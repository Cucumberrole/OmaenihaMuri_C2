#include "VanishingFloor.h"
#include "Player.h"
#include "Telop.h"
#include <DxLib.h>
#include <algorithm>

// 静的メンバの定義
bool VanishingFloor::s_triggered = false;

void VanishingFloor::ResetAll()
{
	s_triggered = false;
}

VanishingFloor::VanishingFloor(int sx, int sy)
{
	floorImage = LoadGraph("data/image/NewBlock.png"); // 床画像
	spikeImage = LoadGraph("data/image/hari.png"); // 針画像

	x = static_cast<float>(sx);
	y = static_cast<float>(sy);

	isActive = true; // 最初は床の画像
	vanishRange = 64.0f + 32.0f; //床が消える範囲
}

VanishingFloor::~VanishingFloor()
{
	DeleteGraph(floorImage);
	DeleteGraph(spikeImage);
}

void VanishingFloor::Update()
{
	Telop* telop = FindGameObject<Telop>();
	Player* player = FindGameObject<Player>();
	if (!player) return;

	float px = player->GetX();
	float py = player->GetY();

	// --- 近づき判定はヒット円の中心で行う ---
	float cx, cy, cr;
	player->GetHitCircle(cx, cy, cr);

	if (!s_triggered)
	{
		float dx = cx - (x + 32.0f);
		float dy = cy - (y + 32.0f);
		float dist2 = dx * dx + dy * dy;

		if (dist2 < vanishRange * vanishRange)
		{
			s_triggered = true;
		}
	}

	isActive = !s_triggered;

	if (s_triggered)
	{
		float left = x;
		float right = x + 64.0f;
		float top = y;
		float bottom = y + 64.0f;

		float nearestX = max(left, min(cx, right));
		float nearestY = max(top, min(cy, bottom));

		float dx = cx - nearestX;
		float dy = cy - nearestY;

		if (dx * dx + dy * dy <= cr * cr)
		{
			player->ForceDie();
			player->SetDead();
			telop->TouchedTrap6 = true;
		}
	}
}

void VanishingFloor::Draw()
{
	// 「床が生きている」ときはブロック画像、
	// 一度トリガーされた後は針画像を描画
	if (isActive)
	{
		DrawRectGraph(static_cast<int>(x), static_cast<int>(y), 0, 0, 64, 64, floorImage, TRUE);
	}
	else
	{
		DrawGraph(static_cast<int>(x), static_cast<int>(y), spikeImage, TRUE);
	}
}

/* ここから足場としての当たり判定（床が生きているときだけ有効） */

// 下方向（プレイヤーの足と床の上面）
int VanishingFloor::HitCheckDown(int px, int py)
{
	if (!isActive) return 0;          // 床が消えていたら足場にならない

	// px,py がこの床タイルの範囲内かどうか
	if (px < x || px >= x + 64) return 0;

	int localY = static_cast<int>(py - y);
	if (localY >= 0 && localY < 64)
	{
		return localY + 1;            // めり込んだ分だけ押し戻す
	}
	return 0;
}

// 上方向（プレイヤーの頭と床の下面）
int VanishingFloor::HitCheckUp(int px, int py)
{
	if (!isActive) return 0;

	if (px < x || px >= x + 64) return 0;

	int localY = static_cast<int>(py - y);
	if (localY >= 0 && localY < 64)
	{
		return 64 - localY;
	}
	return 0;
}

// 左方向
int VanishingFloor::HitCheckLeft(int px, int py)
{
	if (!isActive) return 0;

	if (py < y || py >= y + 64) return 0;

	int localX = static_cast<int>(px - x);
	if (localX >= 0 && localX < 64)
	{
		return 64 - localX;
	}
	return 0;
}

// 右方向
int VanishingFloor::HitCheckRight(int px, int py)
{
	if (!isActive) return 0;

	if (py < y || py >= y + 64) return 0;

	int localX = static_cast<int>(px - x);
	if (localX >= 0 && localX < 64)
	{
		return localX + 1;
	}
	return 0;
}