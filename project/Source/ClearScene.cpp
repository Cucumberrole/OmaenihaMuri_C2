#include "ClearScene.h"
#include "../Library/SceneManager.h"
#include "../Library/Time.h"
#include <DxLib.h>

// --------- static 変数の実体 ---------
float ClearScene::s_clearTime = 0.0f;
int   ClearScene::s_retryCount = 0;
bool  ClearScene::s_noMiss = false;
int   ClearScene::s_finalScore = 0;

// --------- PlayScene から結果を渡す ---------
void ClearScene::SetResult(float clearTime, int retryCount, bool noMiss)
{
	s_clearTime = clearTime;
	s_retryCount = retryCount;
	s_noMiss = noMiss;
}

// --------- コンストラクタ ---------
ClearScene::ClearScene()
{
	CalcScoreAndRank();
}

ClearScene::~ClearScene()
{
}

// スコア計算＋ランク決定
void ClearScene::CalcScoreAndRank()
{
	// ① 基本スコア : 10000 点
	int score = 10000;

	// ② 時間減点 : 1秒ごとに -10 点
	//   例 : clearTime = 123.4 秒 → 1234点マイナス
	int timePenalty = (int)(s_clearTime * 10.0f);
	score -= timePenalty;

	// ③ リトライ減点 : 1回死ぬごとに -500 点
	int retryPenalty = s_retryCount * 500;
	score -= retryPenalty;

	// ④ ボーナス
	if (s_noMiss)
	{
		score += 2000;  // ノーミスクリア
	}
	if (s_clearTime <= 60.0f)
	{
		score += 1000;  // 1分以内クリア
	}

	// 下限 0 にしておく
	if (score < 0) score = 0;

	s_finalScore = score;

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
		oneLineMsg = "出直してきてね♡";
		break;
	}
}

// --------- Update ---------
void ClearScene::Update()
{
	// Tキーでタイトルへ戻る
	if (CheckHitKey(KEY_INPUT_T))
	{
		// タイトルに戻る前に、ここで「ステージだけリセットして残機は維持」
		// などの処理をしたい場合は、StatusManager 的なクラスを呼ぶ。

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
	DrawBox(0, 0, Screen::WIDTH, Screen::HEIGHT, GetColor(255, 255, 255), TRUE);

	// 外枠（お好み）
	DrawBox(40, 40, Screen::WIDTH - 40, Screen::HEIGHT - 40, GetColor(0, 0, 128), FALSE);

	// 左右のキャラ枠（今はただの色付き四角）
	int charBoxW = 260;
	int charBoxH = 420;
	int charBoxY = 140;

	int leftX = 80;
	int rightX = Screen::WIDTH - 80 - charBoxW;

	int charColor = GetColor(255, 204, 0); // 黄色っぽい

	DrawBox(leftX, charBoxY, leftX + charBoxW, charBoxY + charBoxH, charColor, TRUE);
	DrawBox(rightX, charBoxY, rightX + charBoxW, charBoxY + charBoxH, charColor, TRUE);

	SetFontSize(72);
	int titleColor = GetColor(255, 204, 0);

	const char* titleStr = "GAME  CLEAR";
	int wTitle = GetDrawStringWidth(titleStr, -1);
	DrawString((Screen::WIDTH - wTitle) / 2, 80, titleStr, titleColor);

	SetFontSize(48);
	const char* thanksStr = "Thank you  for  Playing";
	int wThanks = GetDrawStringWidth(thanksStr, -1);
	DrawString((Screen::WIDTH - wThanks) / 2, 170, thanksStr, titleColor);

	// ランク表示
	SetFontSize(40);
	int wRankLabel = GetDrawStringWidth("ランク", -1);
	DrawString((Screen::WIDTH - wRankLabel) / 2 - 150, 240, "ランク", GetColor(0, 0, 0));

	int wRank = GetDrawStringWidth(rankText.c_str(), -1);
	DrawString((Screen::WIDTH - wRank) / 2 + 50, 240, rankText.c_str(), rankColor);

	// クリアタイム表示
	SetFontSize(32);

	int minutes = (int)(s_clearTime) / 60;
	int seconds = (int)(s_clearTime) % 60;

	char timeBuf[64];
	sprintf_s(timeBuf, "クリアタイム : %02d:%02d", minutes, seconds);
	int wTime = GetDrawStringWidth(timeBuf, -1);
	DrawString((Screen::WIDTH - wTime) / 2, 300, timeBuf, GetColor(0, 0, 0));

	// リトライ回数
	char retryBuf[64];
	sprintf_s(retryBuf, "リトライ回数 : %d", s_retryCount);
	int wRetry = GetDrawStringWidth(retryBuf, -1);
	DrawString((Screen::WIDTH - wRetry) / 2, 340, retryBuf, GetColor(0, 0, 0));

	// スコア
	char scoreBuf[64];
	sprintf_s(scoreBuf, "スコア : %d", s_finalScore);
	int wScore = GetDrawStringWidth(scoreBuf, -1);
	DrawString((Screen::WIDTH - wScore) / 2, 380, scoreBuf, GetColor(0, 0, 0));

	// 一言メッセージ
	SetFontSize(32);
	int wMsg = GetDrawStringWidth(oneLineMsg.c_str(), -1);
	DrawString((Screen::WIDTH - wMsg) / 2, 430, oneLineMsg.c_str(), GetColor(0, 0, 0));

	// 下部メッセージ
	SetFontSize(28);
	const char* backMsg = "[タイトルに戻る] Tキーを押してください";
	int wBack = GetDrawStringWidth(backMsg, -1);
	DrawString((Screen::WIDTH - wBack) / 2, Screen::HEIGHT - 80, backMsg, GetColor(0, 0, 0));
}