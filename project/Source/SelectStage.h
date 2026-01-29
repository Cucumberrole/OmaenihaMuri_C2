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
	float fade_ = 0.0f;       // 0～1
	int blink_ = 0;

	// あにめーしょん！
	int   introStartMs_ = 0;   // 登場開始時刻
	float introT_ = 0.0f;      // 0.0～1.0
	bool  introDone_ = false;  // 完了フラグ

	// カード表示アニメーション
	float introElapsedSec_ = 0.0f;

	// Background
	int wallImg_ = -1;
	float wallScroll_ = 0.0f;

	// Fonts
	int fontTitle_ = -1;
	int fontSub_ = -1;
	int fontCard_ = -1;
	int fontHint_ = -1;

	// パーティクル
	struct Sparkle { float x, y, v, s; int kind; };
	static const int kSparkleCount = 70;
	Sparkle sp_[kSparkleCount]{};

	// 画面のビネット
	int vignetteImg_ = -1;
	int vignetteSrcW_ = 0;
	int vignetteSrcH_ = 0;
	int vignetteTargetW_ = 0;
	int vignetteTargetH_ = 0;

	//BGM
	int SelectBGM;
	int SelectSE;
	

	int prevSelected = selected_;
};
