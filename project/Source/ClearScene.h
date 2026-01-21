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

private:
	void CalcScoreAndRank();

	void InitConfetti();
	void UpdateConfetti();
	void DrawBackground() const;
	void DrawConfetti() const;

	void DrawPanel(int x, int y, int w, int h) const;
	void DrawOutlinedText(int x, int y, const char* text, unsigned int textColor, unsigned int outlineColor) const;

	void FormatTime(char out[32]) const;
};
