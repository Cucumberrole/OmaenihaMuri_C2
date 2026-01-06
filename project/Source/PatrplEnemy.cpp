#include "PatrolEnemy.h"
#include "Player.h"
#include "Field.h"
#include <DxLib.h>
#include <algorithm>

PatrolEnemy::PatrolEnemy(float sx, float sy, float spd)
{
	x = sx;
	y = sy;
	speed = spd;
	vx = -speed;

	hImage = LoadGraph("data/image/Head.png"); // <- prepare any 64x64 image
	SetDrawOrder(40);
}

PatrolEnemy::~PatrolEnemy()
{
	if (hImage != -1) DeleteGraph(hImage);
}

bool PatrolEnemy::IsSolidAt(float px, float py) const
{
	Field* field = FindGameObject<Field>();
	if (!field) return false;

	int tx = (int)(px) / 64;
	int ty = (int)(py) / 64;
	return field->IsBlock(tx, ty);
}

bool PatrolEnemy::HitPlayer() const
{
	Player* player = FindGameObject<Player>();
	if (!player) return false;

	float px = player->GetX();
	float py = player->GetY();

	// Player assumed 64x64 like your traps
	float pw = 64.0f, ph = 64.0f;

	// AABB vs AABB
	float l1 = x, r1 = x + w;
	float t1 = y, b1 = y + h;
	float l2 = px, r2 = px + pw;
	float t2 = py, b2 = py + ph;

	return (l1 < r2 && r1 > l2 && t1 < b2 && b1 > t2);
}

void PatrolEnemy::Update()
{
	// touch = death
	if (HitPlayer())
	{
		if (Player* p = FindGameObject<Player>())
		{
			p->ForceDie();
			p->SetDead();
		}
		return;
	}

	// next position
	float nextX = x + vx;

	// wall check (two points: upper/lower front)
	float frontX = (vx > 0) ? (nextX + w) : nextX;
	float topY = y + 8.0f;
	float botY = y + h - 8.0f;

	bool hitWall = IsSolidAt(frontX, topY) || IsSolidAt(frontX, botY);

	// edge check (tile under front foot)
	float footX = (vx > 0) ? (nextX + w - 8.0f) : (nextX + 8.0f);
	float footY = y + h + 2.0f;
	bool noFloorAhead = !IsSolidAt(footX, footY);

	if (hitWall || noFloorAhead)
	{
		vx = -vx; // turn around
		nextX = x + vx;
	}

	x = nextX;
}

void PatrolEnemy::Draw()
{
	if (hImage != -1)
	{
		int imgW = 0, imgH = 0;
		GetGraphSize(hImage, &imgW, &imgH);

		// draw scaled to 64x64
		if (vx >= 0)
		{
			DrawExtendGraph((int)x, (int)y, (int)(x + 64), (int)(y + 64), hImage, TRUE);
		}
		else
		{
			// flip horizontally while scaling
			DrawRectExtendGraph(
				(int)x, (int)y, (int)(x + 64), (int)(y + 64),
				imgW, 0, -imgW, imgH,   // srcX=imgW, srcW=-imgW => horizontal flip
				hImage, TRUE
			);
		}
	}
	else
	{
		DrawBox((int)x, (int)y, (int)x + 64, (int)y + 64, GetColor(255, 0, 0), TRUE);
	}
}