#pragma once
#include "../Library/SceneBase.h"

class ClearScene : public SceneBase
{
public:
	ClearScene();
	~ClearScene();

	void Update() override;
	void Draw() override;

private:
	int leftCharaImg;   // 左キャラ画像
	int rightCharaImg;  // 右キャラ画像

	// ランク情報
	int   finalScore;     // 最終スコア
	float clearTime;      // クリアタイム(秒)
	int   retryCount;     // リトライ回数
	char  rank;           // 'S' 'A' 'B' 'C' 'D'

	void CalcRank();                 // スコア→ランクを計算
	const char* GetRankMessage() const; // ランクに応じた一言メッセージ
};