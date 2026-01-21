#include "BouncingAnimatedTrap.h"
#include "Player.h"
#include "Field.h"

int BouncingAnimatedTrap::sImage = -1;
int BouncingAnimatedTrap::sRefCount = 0;

namespace
{
	constexpr const char* kImagePath = "data/image/sui.png";

	static void SetVelocityFromDir(BouncingAnimatedTrap::Dir dir, float speed, float& outVx, float& outVy)
	{
		outVx = 0.0f;
		outVy = 0.0f;
		switch (dir)
		{
		case BouncingAnimatedTrap::Dir::Right:     outVx = +speed; break;
		case BouncingAnimatedTrap::Dir::Left:      outVx = -speed; break;
		case BouncingAnimatedTrap::Dir::Down:      outVy = +speed; break;
		case BouncingAnimatedTrap::Dir::Up:        outVy = -speed; break;
		case BouncingAnimatedTrap::Dir::DownRight: outVx = +speed; outVy = +speed; break;
		case BouncingAnimatedTrap::Dir::DownLeft:  outVx = -speed; outVy = +speed; break;
		case BouncingAnimatedTrap::Dir::UpRight:   outVx = +speed; outVy = -speed; break;
		case BouncingAnimatedTrap::Dir::UpLeft:    outVx = -speed; outVy = -speed; break;
		default: outVx = +speed; break;
		}
	}
}

BouncingAnimatedTrap::BouncingAnimatedTrap(float x, float y, Dir dir, float speed)
{
	this->x = x;
	this->y = y;

	SetVelocityFromDir(dir, speed, vx, vy);

	EnsureSharedImageLoaded();
}

BouncingAnimatedTrap::~BouncingAnimatedTrap()
{
	ReleaseSharedImage();
}

void BouncingAnimatedTrap::EnsureSharedImageLoaded()
{
	if (sImage == -1)
	{
		sImage = LoadGraph(kImagePath);
	}
	sRefCount++;
}

void BouncingAnimatedTrap::ReleaseSharedImage()
{
	sRefCount--;
	if (sRefCount <= 0)
	{
		if (sImage != -1)
		{
			DeleteGraph(sImage);
			sImage = -1;
		}
		sRefCount = 0;
	}
}

bool BouncingAnimatedTrap::HitPlayer(Player* player) const
{
	if (!player) return false;

	// Player is assumed 64x64 (same as other code)
	const float pw = 64.0f;
	const float ph = 64.0f;

	float px = player->GetX();
	float py = player->GetY();

	// AABB vs AABB
	return !(x + w <= px || px + pw <= x || y + h <= py || py + ph <= y);
}

void BouncingAnimatedTrap::AdvanceAnimation()
{
	animFrame++;
	if (animFrame >= ANIM_FRAME_INTERVAL)
	{
		animFrame = 0;
		animIndex = (animIndex + 1) % 2;
	}
}



void BouncingAnimatedTrap::ResolveTileBounceX(Field* field)
{
	if (!field) return;

	// Check 2 points on moving edge
	if (vx > 0.0f)
	{
		int tileX = int(x + w - 1) / 64;
		int tileY1 = int(y + 1) / 64;
		int tileY2 = int(y + h - 2) / 64;

		if (field->IsBlock(tileX, tileY1) || field->IsBlock(tileX, tileY2))
		{
			// Snap to left of the blocking tile
			x = float(tileX * 64 - int(w));
			vx = -vx;
		}
	}
	else if (vx < 0.0f)
	{
		int tileX = int(x) / 64;
		int tileY1 = int(y + 1) / 64;
		int tileY2 = int(y + h - 2) / 64;

		if (field->IsBlock(tileX, tileY1) || field->IsBlock(tileX, tileY2))
		{
			// Snap to right of the blocking tile
			x = float((tileX + 1) * 64);
			vx = -vx;
		}
	}
}

void BouncingAnimatedTrap::ResolveTileBounceY(Field* field)
{
	if (!field) return;

	if (vy > 0.0f)
	{
		int tileY = int(y + h - 1) / 64;
		int tileX1 = int(x + 1) / 64;
		int tileX2 = int(x + w - 2) / 64;

		if (field->IsBlock(tileX1, tileY) || field->IsBlock(tileX2, tileY))
		{
			y = float(tileY * 64 - int(h));
			vy = -vy;
		}
	}
	else if (vy < 0.0f)
	{
		int tileY = int(y) / 64;
		int tileX1 = int(x + 1) / 64;
		int tileX2 = int(x + w - 2) / 64;

		if (field->IsBlock(tileX1, tileY) || field->IsBlock(tileX2, tileY))
		{
			y = float((tileY + 1) * 64);
			vy = -vy;
		}
	}
}

void BouncingAnimatedTrap::Update()
{
	Player* player = FindGameObject<Player>();
	Field* field = FindGameObject<Field>();

	// Move (separate axis for stable bounce)
	x += vx;
	ResolveTileBounceX(field);

	y += vy;
	ResolveTileBounceY(field);

	AdvanceAnimation();

	// Player hit => kill (no other collision)
	if (player && HitPlayer(player))
	{
		player->ForceDie();
		player->SetDead();
	}
}

void BouncingAnimatedTrap::Draw()
{
	if (sImage == -1) return;

	int srcX = animIndex * FRAME_W;
	int srcY = 0;

	DrawRectExtendGraph(
		(int)x, (int)y,
		(int)x + 64, (int)y + 64,
		srcX, srcY,
		FRAME_W, FRAME_H,
		sImage,
		TRUE
	);

#ifdef _DEBUG
	DrawBox((int)x, (int)y, (int)(x + w), (int)(y + h), GetColor(0, 255, 255), FALSE);
#endif
}