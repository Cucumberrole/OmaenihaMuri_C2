#pragma once
#include <DxLib.h>
#include <tchar.h>
#include "../Library/SceneBase.h"

class SelectStage final : public SceneBase
{
public:
	SelectStage();
	~SelectStage() override;

	void Update() override;
	void Draw() override;

private:
	void Decide(int stageId);
	void DrawTiledWall(int sw, int sh) const;
	void DrawVignette(int sw, int sh) const;
	void DrawSparkles(int sw, int sh) const;

private:
	int selected_ = 0;        // 0: Easy, 1: Hard
	bool deciding_ = false;
	float fade_ = 0.0f;       // 0..1
	int blink_ = 0;

	// Background
	int wallImg_ = -1;
	float wallScroll_ = 0.0f;

	// Fonts (Japanese-friendly)
	int fontTitle_ = -1;
	int fontSub_ = -1;
	int fontCard_ = -1;
	int fontHint_ = -1;

	// Sparkles
	struct Sparkle { float x, y, v, s; int kind; };
	static const int kSparkleCount = 70;
	Sparkle sp_[kSparkleCount]{};
};
