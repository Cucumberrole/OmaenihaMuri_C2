#pragma once
#include "../Library/GameObject.h"
#include <DxLib.h>

class Player;
class Field;

// BouncingAnimatedTrap
// - Constant velocity movement
// - Bounces off solid tiles (Field::IsBlock)
// - Collides only with Player (kills player on touch)
// - Sprite sheet: 1280x640 (2 frames side-by-side, each 640x640), drawn as 64x64
class BouncingAnimatedTrap : public GameObject
{
public:
	enum class Dir
	{
		Right,
		Left,
		Down,
		Up,
		DownRight,
		DownLeft,
		UpRight,
		UpLeft,
	};

	BouncingAnimatedTrap(float x, float y, Dir dir = Dir::Right, float speed = 6.0f);
	~BouncingAnimatedTrap() override;

	void Update() override;
	void Draw() override;

private:
	static void EnsureSharedImagesLoaded();
	static void ReleaseSharedImages();

	void AdvanceAnimation();
	void ResolveTileBounceX(Field* field);
	void ResolveTileBounceY(Field* field);
	bool HitPlayer(Player* player) const;

private:
	// Shared resources
	static int sSheetImage;      // fallback sheet handle (LoadGraph)
	static int sFrameImages[2];  // preferred frame handles (LoadDivGraph or DerivationGraph)
	static int sRefCount;

	// World position (top-left)
	float x = 0.0f;
	float y = 0.0f;

	// Size (always 64x64)
	float w = 64.0f;
	float h = 64.0f;

	// Velocity
	float vx = 0.0f;
	float vy = 0.0f;

	// Animation
	int animIndex = 0;   // 0 or 1
	int animFrame = 0;

	static constexpr int kDrawW = 64;
	static constexpr int kDrawH = 64;

	static constexpr int kFrameW = 640;
	static constexpr int kFrameH = 640;

	static constexpr int kAnimInterval = 4;
};
