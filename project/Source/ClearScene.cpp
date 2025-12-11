#include "ClearScene.h"
#include "../Library/SceneManager.h"
#include "../Library/Time.h"
#include "Screen.h"
#include "GameResult.h"

#include <DxLib.h>

ClearScene::ClearScene()
{
	// 結果をコピー
	finalScore = g_GameResult.score;
	clearTime = g_GameResult.clearTime;
	retryCount = g_GameResult.retryCount;

	// ランク計算
	CalcRank();

	// キャラ画像（画像は好きなのに変えてね）
	leftCharaImg = LoadGraph("data/image/すい1.png");
	rightCharaImg = LoadGraph("data/image/すい2.png");
}

ClearScene::~ClearScene()
{
	DeleteGraph(leftCharaImg);
	DeleteGraph(rightCharaImg);
}

void ClearScene::CalcRank()
{
	// ここでは「finalScore」はすでに
	//   10000 - 経過時間*10 - 死亡回数*500 + ボーナス
	// などで計算済みと想定して、その点数だけでランクを決める

	if (finalScore >= 9600) rank = 'S';
	else if (finalScore >= 8500) rank = 'A';
	else if (finalScore >= 7000) rank = 'B';
	else if (finalScore >= 5000) rank = 'C';
	else                         rank = 'D';
}

const char* ClearScene::GetRankMessage() const
{
	switch (rank)
	{
	case 'S': return "完璧すぎる…！もう制作者じゃん";
	case 'A': return "素晴らしい腕前！あと少しでSランク！";
	case 'B': return "ナイスクリア！次はノーミスを目指そう";
	case 'C': return "クリアおめでとう！慣れてきたらタイムアタックも";
	case 'D': return "とにかくクリアは偉い。次はもっとやれるはず！";
	}
	return "";
}

void ClearScene::Update()
{
	// Tキーでタイトルへ戻る
	if (CheckHitKey(KEY_INPUT_T))
	{
		SceneManager::ChangeScene("TITLE");
	}
}

void ClearScene::Draw()
{
	// 背景を白っぽく塗りつぶす
	DrawBox(0, 0, Screen::WIDTH, Screen::HEIGHT,
		GetColor(255, 255, 255), TRUE);

	// 左右キャラ
	int charW = 256;
	int charH = 256;
	int margin = 80;

	int leftX = margin;
	int leftY = (Screen::HEIGHT - charH) / 2;
	int rightX = Screen::WIDTH - margin - charW;
	int rightY = leftY;

	// キャラ画像が256x256じゃない場合は DrawExtendGraph に変えてもOK
	DrawExtendGraph(leftX, leftY, leftX + charW, leftY + charH,
		leftCharaImg, TRUE);
	DrawExtendGraph(rightX, rightY, rightX + charW, rightY + charH,
		rightCharaImg, TRUE);

	// タイトル「GAME CLEAR」
	SetFontSize(80);
	const char* title = "GAME CLEAR";
	int titleW = GetDrawStringWidth(title, -1);
	DrawString((Screen::WIDTH - titleW) / 2, 80,
		title, GetColor(255, 180, 0));

	// 「Thank you for Playing」
	SetFontSize(40);
	const char* sub = "Thank you for Playing";
	int subW = GetDrawStringWidth(sub, -1);
	DrawString((Screen::WIDTH - subW) / 2, 180,
		sub, GetColor(255, 180, 0));

	// ランク・タイム・スコアなど
	int infoY = 260;
	SetFontSize(32);

	// ランク
	char rankStr[32];
	sprintf_s(rankStr, "RANK : %c", rank);
	int rankW = GetDrawStringWidth(rankStr, -1);
	DrawString((Screen::WIDTH - rankW) / 2, infoY,
		rankStr, GetColor(0, 0, 0));

	// クリアタイム
	char timeStr[64];
	sprintf_s(timeStr, "CLEAR TIME : %.2f sec", clearTime);
	int timeW = GetDrawStringWidth(timeStr, -1);
	DrawString((Screen::WIDTH - timeW) / 2, infoY + 50,
		timeStr, GetColor(0, 0, 0));

	// スコア
	char scoreStr[64];
	sprintf_s(scoreStr, "SCORE : %d", finalScore);
	int scoreW = GetDrawStringWidth(scoreStr, -1);
	DrawString((Screen::WIDTH - scoreW) / 2, infoY + 100,
		scoreStr, GetColor(0, 0, 0));

	// 一言メッセージ
	const char* msg = GetRankMessage();
	int msgW = GetDrawStringWidth(msg, -1);
	DrawString((Screen::WIDTH - msgW) / 2, infoY + 160,
		msg, GetColor(0, 0, 0));

	// 下部の「Tキーでタイトル」メッセージ
	SetFontSize(28);
	const char* backMsg = "[タイトルに戻る]  Tキーを押してください";
	int backW = GetDrawStringWidth(backMsg, -1);
	DrawString((Screen::WIDTH - backW) / 2, Screen::HEIGHT - 80,
		backMsg, GetColor(0, 0, 0));
}