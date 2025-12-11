#pragma once
#include "../Library/SceneBase.h"
#include "../Library/GameObject.h"
#include "Screen.h"
#include <string>

class ClearScene : public SceneBase
{
public:
	ClearScene();
	~ClearScene();

	void Update() override;
	void Draw() override;

	// --- PlayScene から結果を渡すための静的関数 ---
	//  clearTime : クリアタイム[秒]
	//  retryCount: そのステージで死んだ回数
	//  noMiss    : ノーミスなら true
	static void SetResult(float clearTime, int retryCount, bool noMiss);

private:
	// --- クリア結果（全インスタンス共通で保持） ---
	static float s_clearTime;
	static int   s_retryCount;
	static bool  s_noMiss;
	static int   s_finalScore;

	// --- ランク情報 ---
	char        rankChar;      // 'S','A','B','C','D'
	std::string rankText;      // "S RANK" など
	std::string oneLineMsg;    // 一言メッセージ
	unsigned int rankColor;    // ランク文字の色

	// 内部処理
	void CalcScoreAndRank();   // スコア計算＋ランク決定
};