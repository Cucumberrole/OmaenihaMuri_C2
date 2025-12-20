#include "FallingFloor.h"
#include "Player.h"
#include "Field.h"
#include <DxLib.h>
#include <algorithm>

FallingFloor::FallingFloor(int sx, int sy)
{
	hImage = LoadGraph("data/image/NewBlock.png");
	x = static_cast<float>(sx);
	y = static_cast<float>(sy);

	velocityY = 0.0f;
	isFalling = false;
	isLanded = false;
	gravity = 0.8f;

	SetDrawOrder(-1);
}

FallingFloor::~FallingFloor()
{
	DeleteGraph(hImage);
}

void FallingFloor::StartFalling()
{
	isFalling = true;
	velocityY = 0.0f;
}

void FallingFloor::Update()
{
	if (isLanded) return;

	Player* player = FindGameObject<Player>();
	if (!player) return;

	Field* field = FindGameObject<Field>();
	if (!field) return;

	float px = player->GetX();
	float py = player->GetY();

	// --- プレイヤーが真下を通過したら落下開始 ---
	if (!isFalling)
	{
		Field* field = FindGameObject<Field>();
		if (!field) return;

		// 横が重なっているか
		bool overlapX = (px + 64 > x) && (px < x + 64);

		// プレイヤーが床の「下」にいるか（床の下端より下）
		bool isBelow = (py >= y + 64);

		if (overlapX && isBelow)
		{
			// 落ちる床の真下の列(tileX)を調べる
			int tileX = int((x + 32) / 64);

			// 落ちる床の「1つ下」タイルから、プレイヤーのいるタイルまで走査
			int startTy = int((y + 64) / 64);
			int endTy = int((py + 32) / 64);

			bool blocked = false;
			for (int ty = startTy; ty <= endTy; ++ty)
			{
				if (field->IsBlock(tileX, ty))   // 途中にブロックがあった（天井など）
				{
					blocked = true;
					break;
				}
			}

			// 途中に遮るブロックがない＝同じ空間にいる → 発動
			if (!blocked)
			{
				StartFalling();
			}
		}
	}

	// -----------------------------
	// 落下処理
	// -----------------------------
	if (isFalling)
	{
		velocityY += gravity;
		y += velocityY;

		// -----------------------------
		// 落下中にプレイヤーと重なったら潰して死亡（常時判定）
		// -----------------------------
		{
			float fx1 = x;
			float fx2 = x + 64;
			float fy1 = y;
			float fy2 = y + 64;

			float px1 = px;
			float px2 = px + 64;
			float py1 = py;
			float py2 = py + 64;

			bool hit =
				(fx1 < px2) && (fx2 > px1) &&
				(fy1 < py2) && (fy2 > py1) &&
				(velocityY > 0.0f);

			if (hit)
			{
				player->ForceDie();
				player->SetDead();
			}
		}

		// -----------------------------
		// 下のブロックに当たったら停止
		// -----------------------------
		int push1 = field->HitCheckDown((int)(x + 5), (int)(y + 64));
		int push2 = field->HitCheckDown((int)(x + 59), (int)(y + 64));
		int push = max(push1, push2);

		if (push > 0)
		{
			y -= push;          // 押し戻し
			velocityY = 0.0f;
			isFalling = false;
			isLanded = true;
		}
	}
}

void FallingFloor::Draw()
{
	DrawRectGraph((int)x, (int)y, 0, 0, 64, 64, hImage, TRUE);
}

// -----------------------------
// 当たり判定（Player.cpp が使うポイント判定方式）
// -----------------------------
int FallingFloor::HitCheckRight(int px, int py)
{
	if (py < y || py >= y + 64) return 0;
	int localX = (int)(px - x);
	if (localX >= 0 && localX < 64) return localX + 1;
	return 0;
}

int FallingFloor::HitCheckLeft(int px, int py)
{
	if (py < y || py >= y + 64) return 0;
	int localX = (int)(px - x);
	if (localX >= 0 && localX < 64) return 64 - localX;
	return 0;
}

int FallingFloor::HitCheckDown(int px, int py)
{
	if (px < x || px >= x + 64) return 0;
	int localY = (int)(py - y);
	if (localY >= 0 && localY < 64) return localY + 1;
	return 0;
}

int FallingFloor::HitCheckUp(int px, int py)
{
	if (px < x || px >= x + 64) return 0;
	int localY = (int)(py - y);
	if (localY >= 0 && localY < 64) return 64 - localY;
	return 0;
}