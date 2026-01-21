#pragma once
#include "../Library/GameObject.h"
#include <DxLib.h>

// Forward declarations
class Player;
class Field;

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
	static void EnsureSharedImageLoaded();
	static void ReleaseSharedImage();

	void AdvanceAnimation();
	void ResolveTileBounceX(Field* field);
	void ResolveTileBounceY(Field* field);
	bool HitPlayer(Player* player) const;

private:
	static int sImage;
	static int sRefCount;

	// 640x640
	static constexpr int FRAME_W = 640;
	static constexpr int FRAME_H = 640;

	// 初期値
	float x = 0.0f;
	float y = 0.0f;

	// 画像サイズ
	float w = 64.0f;
	float h = 64.0f;

	// ヴェロシティ
	float vx = 0.0f;
	float vy = 0.0f;

	// アニメーション
	int frame = 0;
	int animTimer = 0;
	int animInterval = 4;

	// 画像
	static constexpr int kFrameW = 64;
	static constexpr int kFrameH = 64;
	static constexpr int kSheetW = 1280;
	static constexpr int kSheetH = 640;
	static constexpr int kCols = kSheetW / kFrameW; // 20
	static constexpr int kRows = kSheetH / kFrameH; // 10
	static constexpr int kTotalFrames = kCols * kRows; // 200

	// アニメーション
	int animIndex;        // 現在のコマ番号（0 or 1）
	int animFrame;        // フレームカウンタ
	static const int CHARACTER_WIDTH = 640;   // 1コマの横幅
	static const int CHARACTER_HEIGHT = 640;  // 1コマの縦幅
	static const int ANIM_FRAME_INTERVAL = 4; // コマ切替速度
};
