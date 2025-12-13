#include "ClearScene.h"
#include "GameResult.h"
#include "../Library/SceneManager.h"
#include "../Library/Time.h"
#include <DxLib.h>

// --------- コンストラクタ ---------
ClearScene::ClearScene()
{
    // PlayScene で計算済みの結果をコピー
    clearTime = g_GameResult.clearTime;
    retryCount = g_GameResult.retryCount;
    finalScore = g_GameResult.score;

    // スコアに応じてランクなどを決める
    CalcScoreAndRank();
}

ClearScene::~ClearScene()
{
}

// スコア計算＋ランク決定
void ClearScene::CalcScoreAndRank()
{
    int score = finalScore;

    // --------- ランク判定 ---------
    if (score >= 9600)               rankChar = 'S';
    else if (score >= 8500)          rankChar = 'A';
    else if (score >= 7000)          rankChar = 'B';
    else if (score >= 5000)          rankChar = 'C';
    else                             rankChar = 'D';

    // ランク文言と色・一言メッセージ
    switch (rankChar)
    {
    case 'S':
        rankText = "S RANK";
        rankColor = GetColor(255, 215, 0);   // 金色っぽく
        oneLineMsg = "無理とか言ってすみませんでした…あなたは天才です。";
        break;
    case 'A':
        rankText = "A RANK";
        rankColor = GetColor(192, 192, 192); // 銀色っぽく
        oneLineMsg = "やりますねぇ！　Sランクまであと少し！";
        break;
    case 'B':
        rankText = "B RANK";
        rankColor = GetColor(255, 0, 0);     // 赤
        oneLineMsg = "うん、普通。コメントに困る。";
        break;
    case 'C':
        rankText = "C RANK";
        rankColor = GetColor(0, 0, 255);     // 青
        oneLineMsg = "もう少し真面目にやろっか…？";
        break;
    case 'D':
    default:
        rankText = "D RANK";
        rankColor = GetColor(139, 69, 19);   // 茶色
        oneLineMsg = "出直してきてね💛";
        break;
    }
}

// --------- Update ---------
void ClearScene::Update()
{
    // Tキーでタイトルへ戻る
    if (CheckHitKey(KEY_INPUT_T))
    {
        SceneManager::ChangeScene("TITLE");
    }

    // ESC でゲーム終了
    if (CheckHitKey(KEY_INPUT_ESCAPE))
    {
        SceneManager::Exit();
    }
}

// --------- Draw ---------
void ClearScene::Draw()
{
    // 背景を白で塗りつぶし
    DrawBox(0, 0, Screen::WIDTH, Screen::HEIGHT,
        GetColor(255, 255, 255), TRUE);

    // 外枠
    DrawBox(40, 40, Screen::WIDTH - 40, Screen::HEIGHT - 40,
        GetColor(0, 0, 128), FALSE);

    // 左右のキャラ枠
    int charBoxW = 260;
    int charBoxH = 420;
    int charBoxY = 140;

    int leftX = 80;
    int rightX = Screen::WIDTH - 80 - charBoxW;

    int charColor = GetColor(255, 204, 0); // 黄色

    DrawBox(leftX, charBoxY, leftX + charBoxW, charBoxY + charBoxH, charColor, TRUE);
    DrawBox(rightX, charBoxY, rightX + charBoxW, charBoxY + charBoxH, charColor, TRUE);

    // タイトル
    SetFontSize(72);
    int titleColor = GetColor(255, 204, 0);
    const char* titleStr = "GAME  CLEAR";
    int wTitle = GetDrawStringWidth(titleStr, -1);
    DrawString((Screen::WIDTH - wTitle) / 2, 80, titleStr, titleColor);

    // Thank you
    SetFontSize(48);
    const char* thanksStr = "Thank you  for  Playing";
    int wThanks = GetDrawStringWidth(thanksStr, -1);
    DrawString((Screen::WIDTH - wThanks) / 2, 170, thanksStr, titleColor);

    // ランク表示
    SetFontSize(40);
    int wRankLabel = GetDrawStringWidth("ランク", -1);
    DrawString((Screen::WIDTH - wRankLabel) / 2 - 150, 240,
        "ランク", GetColor(0, 0, 0));

    int wRank = GetDrawStringWidth(rankText.c_str(), -1);
    DrawString((Screen::WIDTH - wRank) / 2 + 50, 240,
        rankText.c_str(), rankColor);

    // クリアタイム
    SetFontSize(32);
    int minutes = (int)(clearTime) / 60;
    int seconds = (int)(clearTime) % 60;

    char timeBuf[64];
    sprintf_s(timeBuf, "クリアタイム : %02d:%02d", minutes, seconds);
    int wTime = GetDrawStringWidth(timeBuf, -1);
    DrawString((Screen::WIDTH - wTime) / 2, 300, timeBuf, GetColor(0, 0, 0));

    // リトライ回数
    char retryBuf[64];
    sprintf_s(retryBuf, "リトライ回数 : %d", retryCount);
    int wRetry = GetDrawStringWidth(retryBuf, -1);
    DrawString((Screen::WIDTH - wRetry) / 2, 340, retryBuf, GetColor(0, 0, 0));

    // スコア
    char scoreBuf[64];
    sprintf_s(scoreBuf, "スコア : %d", finalScore);
    int wScore = GetDrawStringWidth(scoreBuf, -1);
    DrawString((Screen::WIDTH - wScore) / 2, 380, scoreBuf, GetColor(0, 0, 0));

    // 一言メッセージ
    SetFontSize(32);
    int wMsg = GetDrawStringWidth(oneLineMsg.c_str(), -1);
    DrawString((Screen::WIDTH - wMsg) / 2, 430,
        oneLineMsg.c_str(), GetColor(0, 0, 0));

    // 下部メッセージ
    SetFontSize(28);
    const char* backMsg = "[タイトルに戻る] Tキーを押してください";
    int wBack = GetDrawStringWidth(backMsg, -1);
    DrawString((Screen::WIDTH - wBack) / 2, Screen::HEIGHT - 80,
        backMsg, GetColor(0, 0, 0));
}
