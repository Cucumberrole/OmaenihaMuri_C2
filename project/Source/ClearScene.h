#pragma once
#include "../Library/SceneBase.h"
#include "Screen.h"
#include <string>

class ClearScene : public SceneBase
{
public:
    ClearScene();
    ~ClearScene();

    void Update() override;
    void Draw() override;

private:
    // --- クリア結果（このシーン専用の値） ---
    float clearTime;   // クリアタイム(秒)
    int   retryCount;  // リトライ回数
    int   finalScore;  // 最終スコア

    // --- ランク情報 ---
    char        rankChar;      // 'S','A','B','C','D'
    std::string rankText;      // "S RANK" など
    std::string oneLineMsg;    // 一言メッセージ
    unsigned int rankColor;    // ランク文字の色

    // 内部処理
    void CalcScoreAndRank();
};