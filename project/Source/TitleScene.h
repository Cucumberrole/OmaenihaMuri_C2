#pragma once
#include <vector>
#include "../Library/SceneBase.h"

class TitleScene : public SceneBase
{
public:
	TitleScene();
	~TitleScene();

	void Update() override;
	void Draw() override;

private:
	// 背景
	int bgHandle = -1;

	// タイトルロゴ
	int logoHandle = -1;
	int logoW = 0, logoH = 0;

	// ブロック・プレイヤー
	int blockHandle = -1;         // NewBlock.png
	int playerSheetHandle = -1;   // OMAEwalk.png
	int blockW = 64, blockH = 64;

	int playerSheetW = 0, playerSheetH = 0;
	int playerFrameW = 64, playerFrameH = 64;
	int playerFrames = 12;

	// BGM
	int TitleHandle = -1;

	// CSV
	std::vector<std::vector<int>> maps;
	bool mapLoaded = false;

	// プレイヤー座標
	float px = 0.0f;
	float py = 0.0f;
	bool  spawned = false;

	// アニメ
	int   animFrame = 0;
	float animTime = 0.0f;
	bool  flip = false;

	// ロゴ
	float logoX = 0.0f;
	float logoY = 0.0f;
	float logoVY = 0.0f;
	float logoTargetY = 0.0f;
	bool  logoLanded = false;

	// 「push to shift」点滅
	float blinkTime = 0.0f;

	// 火の粉/ほこり
	struct Particle
	{
		float x = 0, y = 0, vx = 0, vy = 0, life = 0, ttl = 0, size = 2;
		int kind = 0; // 0=dust 1=ember
	};
	static constexpr int kParticleCount = 10;
	Particle particles[kParticleCount];

private:
	// CSV読込
	void LoadTitleCsv(const char* path);

	bool IsSolidCell(int tx, int ty) const;
	int  HitCheckRight(int px, int py) const;
	int  HitCheckLeft(int px, int py) const;

	void ResetParticle(int i);
};
