#pragma once
#include "../Library/SceneBase.h"
#include "Screen.h"
#include <string>
#include <vector>

// ClearScene: result screen shown after stage clear.
// Note: Keep public interface compatible with existing SceneBase usage.
class ClearScene : public SceneBase
{
public:
	ClearScene();
	~ClearScene();

	void Update() override;
	void Draw() override;

private:
	// Result values (copied from GameResult)
	float clearTime = 0.0f;   // seconds
	int   retryCount = 0;
	int   finalScore = 0;

	// Rank
	char        rankChar = 'D';      // 'S','A','B','C','D'
	std::string rankText;            // e.g. "S RANK"
	std::string oneLineMsg;          // short message
	unsigned int rankColor = 0;      // DxLib color

private:
	struct Confetti
	{
		float x = 0.0f;
		float y = 0.0f;
		float vx = 0.0f;
		float vy = 0.0f;
		float size = 8.0f;
		unsigned int color = 0;
		int kind = 0; // 0: box, 1: tri, 2: spark
	};

private:
	// Resources
	int imgChar = -1;

	// Animation / particles
	int frame = 0;
	std::vector<Confetti> confetti;

	// ---- Result UI Animation ----
	int   animStartMs_ = 0;
	int   lastMs_ = 0;
	float animTime_ = 0.0f;
	bool  animSkip_ = false;
	bool  animDone_ = false;

	// display values (animated)
	int   dispScore_ = 0;
	float dispTimeSec_ = 0.0f;

	// alpha / motion
	int   panelAlpha_ = 0;     // 0..255
	int   rankAlpha_ = 0;     // 0..255
	float rankOffsetY_ = -20.0f;

private:
	void CalcScoreAndRank();

	void InitConfetti();
	void UpdateConfetti();
	void DrawBackground() const;
	void DrawConfetti() const;

	void DrawPanel(int x, int y, int w, int h) const;
	void DrawOutlinedText(int x, int y, const char* text, unsigned int textColor, unsigned int outlineColor) const;

	void FormatTime(char out[32]) const;

	// Fonts
	int fontTitle_ = -1;      // GAME CLEAR 代替
	int fontThanks_ = -1;     // Thank you 代替
	int fontMsg_ = -1;        // 一言メッセージ
	int fontRankLabel_ = -1;  // "RANK"
	int fontRankValue_ = -1;  // "S" など
	int fontPanel_ = -1;      // CLEAR TIME / SCORE / 値
	int fontHint_ = -1;       // Footer hint

	// アウトライン
	void DrawOutlinedTextToHandle(int x, int y, const char* text, unsigned int textColor, unsigned int outlineColor, int fontHandle) const;

	//BGM
	int GoalBGM;
};
