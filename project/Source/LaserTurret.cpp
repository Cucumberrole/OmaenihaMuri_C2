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

	// 砲台画像（仮：共通のやつ。専用画像があるなら差し替え）
	hTurret = LoadGraph("data/image/houda.png"); // なければあとで用意
	SetDrawOrder(-1);
}

LaserTurret::~LaserTurret()
{
	if (hTurret != -1)
	{
		DeleteGraph(hTurret);
	}
}

void LaserTurret::Update()
{
	animCount++;

	int period = fireInterval + beamDuration;
	if (period <= 0) period = 1;

	// 0～period-1 の中で、先頭 beamDuration フレームだけ撃つ
	int t = animCount % period;
	bool isFiring = (t < beamDuration);

	if (!isFiring) return;

	// --- ここから下は「撃ってる間だけ」の当たり判定 ---

	Player* player = FindGameObject<Player>();
	if (!player) return;

	float cx, cy, cr;
	player->GetHitCircle(cx, cy, cr);
	VECTOR center = VGet(cx, cy, 0.0f);

	// ビームの始点（砲台の中心）
	float sx = x + 32.0f;
	float sy = y + 32.0f;

	// 方向ベクトル
	float dirX = 0.0f;
	float dirY = 0.0f;
	switch (dir)
	{
	case Dir::Right: dirX = 1.0f; dirY = 0.0f; break;
	case Dir::Left:  dirX = -1.0f; dirY = 0.0f; break;
	case Dir::Up:    dirX = 0.0f; dirY = -1.0f; break;
	case Dir::Down:  dirX = 0.0f; dirY = 1.0f; break;
	}

	float ex = sx;
	float ey = sy;

	Field* field = FindGameObject<Field>();

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
				// ぶつかる直前で止める
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

	VECTOR a = VGet(sx, sy, 0.0f);
	VECTOR b = VGet(ex, ey, 0.0f);

	if (HitCheck_Circle_Line(center, cr, a, b))
	{
		player->ForceDie();
		player->SetDead();
	}
}
void LaserTurret::Draw()
{
	// 砲台本体
	if (hTurret != -1)
	{
		DrawGraph((int)x, (int)y, hTurret, TRUE);
	}
	else
	{
		DrawBox((int)x, (int)y, (int)x + 64, (int)y + 64,
			GetColor(128, 128, 128), TRUE);
	}

	int period = fireInterval + beamDuration;
	if (period <= 0) period = 1;
	int t = animCount % period;
	bool isFiring = (t < beamDuration);

	if (!isFiring) return;

	// --- ビーム線 ---
	float sx = x + 32.0f;
	float sy = y + 32.0f;

	float dirX = 0.0f;
	float dirY = 0.0f;
	switch (dir)
	{
	case Dir::Right: dirX = 1.0f; dirY = 0.0f; break;
	case Dir::Left:  dirX = -1.0f; dirY = 0.0f; break;
	case Dir::Up:    dirX = 0.0f; dirY = -1.0f; break;
	case Dir::Down:  dirX = 0.0f; dirY = 1.0f; break;
	}

	float ex = sx;
	float ey = sy;

	Field* field = FindGameObject<Field>();

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

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 192);
	DrawBox(
		(int)min(sx, ex),
		(int)min(sy, ey),
		(int)max(sx, ex),
		(int)max(sy, ey),
		GetColor(0, 255, 255),
		TRUE
	);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}