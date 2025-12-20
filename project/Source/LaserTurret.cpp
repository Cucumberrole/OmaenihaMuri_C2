#include "LaserTurret.h"
#include "Player.h"
#include "Field.h"
#include "Collision.h"
#include <DxLib.h>
#include <algorithm>

LaserTurret::LaserTurret(float sx, float sy, Dir dir)
{
	x = sx;
	y = sy;
	this->dir = dir;

	// 砲台画像（2コマ：通常 / 発射中）
	int heads[2];
	if (LoadDivGraph("data/image/BreathHead.png",
		2, 2, 1,
		64, 64,
		heads) == 0)
	{
		hHead[0] = heads[0]; // 左：通常
		hHead[1] = heads[1]; // 右：発射中
	}

	// レーザー画像
	hLaser = LoadGraph("data/image/BOAAA.png");
	if (hLaser != -1)
	{
		GetGraphSize(hLaser, &laserW, &laserH);
	}

	SetDrawOrder(40);
}

LaserTurret::~LaserTurret()
{
	for (int i = 0; i < 2; ++i)
	{
		if (hHead[i] != -1)
		{
			DeleteGraph(hHead[i]);
			hHead[i] = -1;
		}
	}
	if (hLaser != -1)
	{
		DeleteGraph(hLaser);
		hLaser = -1;
	}
}

void LaserTurret::Update()
{
	animCount++;

	int period = fireInterval + beamDuration;
	if (period <= 0) period = 1;

	int t = animCount % period;
	isFiring = (t < beamDuration);

	// ビーム始点
	float sx = x + 64.0f;
	float sy = y + 32.0f;

	beamStartX = sx;
	beamStartY = sy;
	beamEndX = sx;
	beamEndY = sy;

	// ビーム方向
	float dirX = 0.0f;
	float dirY = 0.0f;
	switch (dir)
	{
	case Dir::Right: dirX = 1.0f; dirY = 0.0f; break;
	case Dir::Left:  dirX = -1.0f; dirY = 0.0f; break;
	case Dir::Up:    dirX = 0.0f; dirY = -1.0f; break;
	case Dir::Down:  dirX = 0.0f; dirY = 1.0f; break;
	}

	Field* field = FindGameObject<Field>();

	float ex = sx;
	float ey = sy;

	const float step = 32.0f;
	const int   maxSteps = 100;

	for (int i = 0; i < maxSteps; ++i)
	{
		ex += dirX * step;
		ey += dirY * step;

		if (field)
		{
			int tx = static_cast<int>(ex) / 64;
			int ty = static_cast<int>(ey) / 64;
			if (field->IsBlock(tx, ty))
			{
				ex -= dirX * step;
				ey -= dirY * step;
				break;
			}
		}

		float dx = ex - sx;
		float dy = ey - sy;
		if (dx * dx + dy * dy >= maxLength * maxLength)
		{
			break;
		}
	}

	beamEndX = ex;
	beamEndY = ey;

	// 撃っていないときはダメージなし
	if (!isFiring) return;

	// --- 当たり判定 ---
	Player* player = FindGameObject<Player>();
	if (!player) return;

	float cx, cy, cr;
	player->GetHitCircle(cx, cy, cr);
	VECTOR center = VGet(cx, cy, 0.0f);

	VECTOR a = VGet(beamStartX, beamStartY, 0.0f);
	VECTOR b = VGet(beamEndX, beamEndY, 0.0f);

	if (HitCheck_Circle_Line(center, cr, a, b))
	{
		player->ForceDie();
		player->SetDead();
	}
}

void LaserTurret::Draw()
{
	// 砲台本体
	int headIndex = isFiring ? 1 : 0;
	int headHandle = hHead[headIndex];

	// とりあえず右向き画像前提で描画
	// 左向きにしたいときは左右反転（DrawTurnGraph）を使う
	if (headHandle != -1)
	{
		if (dir == Dir::Right)
		{
			DrawGraph(x, y, headHandle, TRUE);
		}
		else if (dir == Dir::Left)
		{
			// 左右反転
			DrawTurnGraph(x, y, headHandle, TRUE);
		}
		else
		{
			// 上下はとりあえずそのまま（必要なら後で回転対応）
			DrawGraph(x, y, headHandle, TRUE);
		}
	}
	else
	{
		DrawBox(x, y, x + 64, y + 64, GetColor(128, 128, 128), TRUE);
	}

	// ビーム描画
	if (!isFiring) return;
	if (hLaser == -1) return;

	float sx = beamStartX;
	float sy = beamStartY;
	float ex = beamEndX;
	float ey = beamEndY;

	// 今回は横方向（Right/Left）前提で画像を伸ばす
	if (dir == Dir::Right || dir == Dir::Left)
	{
		float yTop = sy - laserH * 0.5f;
		float yBot = sy + laserH * 0.5f;

		float left = min(sx, ex);
		float right = max(sx, ex);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 240);
		DrawRectExtendGraph(left, yTop, right, yBot, 0, 0, laserW, laserH, hLaser, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else
	{
		// 上下方向のときは、ひとまず線で描く（画像回転は後で追加してもOK）
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 240);
		DrawLine(sx, sy, ex, ey, GetColor(255, 255, 255), 8);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}
