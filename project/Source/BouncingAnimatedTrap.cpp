#include "BouncingAnimatedTrap.h"
#include "Player.h"
#include "Field.h"

namespace
{
	constexpr const char* kImagePath = "data/image/sui.png";

	static void SetVelocityFromDir(BouncingAnimatedTrap::Dir dir, float speed, float& outVx, float& outVy)
	{
		outVx = 0.0f;
		outVy = 0.0f;

		switch (dir)
		{
		case BouncingAnimatedTrap::Dir::Right:     outVx = +speed; outVy = 0.0f; break;
		case BouncingAnimatedTrap::Dir::Left:      outVx = -speed; outVy = 0.0f; break;
		case BouncingAnimatedTrap::Dir::Down:      outVx = 0.0f;   outVy = +speed; break;
		case BouncingAnimatedTrap::Dir::Up:        outVx = 0.0f;   outVy = -speed; break;
		case BouncingAnimatedTrap::Dir::DownRight: outVx = +speed; outVy = +speed; break;
		case BouncingAnimatedTrap::Dir::DownLeft:  outVx = -speed; outVy = +speed; break;
		case BouncingAnimatedTrap::Dir::UpRight:   outVx = +speed; outVy = -speed; break;
		case BouncingAnimatedTrap::Dir::UpLeft:    outVx = -speed; outVy = -speed; break;
		}
	}

	static inline int ClampTile(int v)
	{
		return v < 0 ? 0 : v;
	}
}

int BouncingAnimatedTrap::sSheetImage = -1;
int BouncingAnimatedTrap::sFrameImages[2] = { -1, -1 };
int BouncingAnimatedTrap::sRefCount = 0;

BouncingAnimatedTrap::BouncingAnimatedTrap(float x, float y, Dir dir, float speed)
{
	this->x = x;
	this->y = y;

	SetVelocityFromDir(dir, speed, vx, vy);

	EnsureSharedImagesLoaded();
}

BouncingAnimatedTrap::~BouncingAnimatedTrap()
{
	ReleaseSharedImages();
}

void BouncingAnimatedTrap::EnsureSharedImagesLoaded()
{
	// Reference-counted: load once, free when last instance is destroyed.
	if (sRefCount == 0)
	{
		// Preferred: split into 2 frame handles (each 640x640)
		int tmp[2] = { -1, -1 };
		const int ok = LoadDivGraph(kImagePath, 2, 2, 1, kFrameW, kFrameH, tmp);

		if (ok != 0)
		{
			// If LoadDivGraph fails, fall back to LoadGraph + DerivationGraph (or DrawRectExtendGraph)
			sSheetImage = LoadGraph(kImagePath);
			if (sSheetImage != -1)
			{
				// Derive 2 frames from the sheet (safer than rect-drawing in some setups)
				sFrameImages[0] = DerivationGraph(0, 0, kFrameW, kFrameH, sSheetImage);
				sFrameImages[1] = DerivationGraph(kFrameW, 0, kFrameW, kFrameH, sSheetImage);
			}
		}
		else
		{
			// Success
			sFrameImages[0] = tmp[0];
			sFrameImages[1] = tmp[1];
			sSheetImage = -1; // not used
		}

#ifdef _DEBUG
		if (sFrameImages[0] == -1 || sFrameImages[1] == -1)
		{
			// ASCII only (avoid mojibake)
			printfDx("BouncingAnimatedTrap: Image load failed: %s\n", kImagePath);
		}
#endif
	}

	sRefCount++;
}

void BouncingAnimatedTrap::ReleaseSharedImages()
{
	sRefCount--;
	if (sRefCount > 0) return;

	// Free frames first
	for (int i = 0; i < 2; ++i)
	{
		if (sFrameImages[i] != -1)
		{
			DeleteGraph(sFrameImages[i]);
			sFrameImages[i] = -1;
		}
	}

	// Free fallback sheet
	if (sSheetImage != -1)
	{
		DeleteGraph(sSheetImage);
		sSheetImage = -1;
	}

	sRefCount = 0;
}

bool BouncingAnimatedTrap::HitPlayer(Player* player) const
{
	if (!player) return false;

	const float px = player->GetX();
	const float py = player->GetY();
	const float pw = player->GetW();
	const float ph = player->GetH();

	// AABB vs AABB
	return !(x + w <= px || px + pw <= x || y + h <= py || py + ph <= y);
}

void BouncingAnimatedTrap::AdvanceAnimation()
{
	animFrame++;
	if (animFrame >= kAnimInterval)
	{
		animFrame = 0;
		animIndex = (animIndex + 1) % 2;
	}
}

void BouncingAnimatedTrap::ResolveTileBounceX(Field* field)
{
	if (!field) return;

	if (vx > 0.0f)
	{
		const int tileX = ClampTile(int(x + w - 1) / 64);
		const int tileY1 = ClampTile(int(y + 1) / 64);
		const int tileY2 = ClampTile(int(y + h - 2) / 64);

		if (field->IsBlock(tileX, tileY1) || field->IsBlock(tileX, tileY2))
		{
			x = float(tileX * 64 - int(w));
			vx = -vx;
		}
	}
	else if (vx < 0.0f)
	{
		const int tileX = ClampTile(int(x) / 64);
		const int tileY1 = ClampTile(int(y + 1) / 64);
		const int tileY2 = ClampTile(int(y + h - 2) / 64);

		if (field->IsBlock(tileX, tileY1) || field->IsBlock(tileX, tileY2))
		{
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
		const int tileY = ClampTile(int(y + h - 1) / 64);
		const int tileX1 = ClampTile(int(x + 1) / 64);
		const int tileX2 = ClampTile(int(x + w - 2) / 64);

		if (field->IsBlock(tileX1, tileY) || field->IsBlock(tileX2, tileY))
		{
			y = float(tileY * 64 - int(h));
			vy = -vy;
		}
	}
	else if (vy < 0.0f)
	{
		const int tileY = ClampTile(int(y) / 64);
		const int tileX1 = ClampTile(int(x + 1) / 64);
		const int tileX2 = ClampTile(int(x + w - 2) / 64);

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

	if (player && HitPlayer(player))
	{
		player->ForceDie();
		player->SetDead();
	}
}

void BouncingAnimatedTrap::Draw()
{
	// If image not available, draw a debug placeholder in _DEBUG.
	if (sFrameImages[0] == -1 || sFrameImages[1] == -1)
	{
#ifdef _DEBUG
		DrawBox((int)x, (int)y, (int)(x + w), (int)(y + h), GetColor(255, 0, 0), FALSE);
		DrawString((int)x, (int)y, "IMG?", GetColor(255, 0, 0));
#endif
		return;
	}

	const int handle = sFrameImages[animIndex];

	// Alternative display method: draw the frame handle directly, scaled to 64x64.
	DrawExtendGraph(
		(int)x, (int)y,
		(int)x + kDrawW, (int)y + kDrawH,
		handle,
		TRUE
	);

#ifdef _DEBUG
	DrawBox((int)x, (int)y, (int)(x + w), (int)(y + h), GetColor(0, 255, 255), FALSE);
#endif
}
