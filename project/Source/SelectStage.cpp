#include "SelectStage.h"
#include "PlayScene.h"
#include "Screen.h"
#include "Fader.h"
#include "../Library/SceneManager.h"
#include "../Library/Trigger.h"
#include <DxLib.h>


SelectStage::SelectStage()
{
	backgroundImage = LoadGraph("data/image/WhiteBack.png");

	// 表示するステージ名
	stageNames = { "STAGE 1", "STAGE 2", "STAGE 3", "DEBUG STAGE" };
	stageEnabled = { true,true,false,false };

	cursor = 0;

	rectSelect = RECT_LEFT;

	blinkTimer = 0;
	blinkOn = true;
}

SelectStage::~SelectStage()
{
}

void SelectStage::Update()
{
	Fader* fader = FindGameObject<Fader>();

	// --- 上移動 ---
	if (KeyTrigger::CheckTrigger(KEY_INPUT_LEFT)) {
		rectSelect = RECT_LEFT;
	}

	// --- 下移動 ---
	if (KeyTrigger::CheckTrigger(KEY_INPUT_RIGHT)) {
		rectSelect = RECT_RIGHT;
	}

	if (KeyTrigger::CheckTrigger(KEY_INPUT_B)) {
		// ボスステージの開放
		stageEnabled[2] = true;
	}

	if (KeyTrigger::CheckTrigger(KEY_INPUT_D)) {
		// デバッグステージの開放
		stageEnabled[3] = true;
	}

	// --- 決定でステージへ ---
	if (KeyTrigger::CheckTrigger(KEY_INPUT_RETURN)) {

		//if (stageEnabled[cursor])    // 有効なときだけ
		//{
		//	// デバッグステージ（cursor == 2）の時だけ、ステージナンバーを 0 に設定する
		//	if (cursor == 3)
		//	{
		//		PlayScene::SelectedStage = 0;
		//	}
		//	else
		//	{
		//		// STAGE 1, STAGE 2 の場合は、これまで通り cursor + 1
		//		PlayScene::SelectedStage = cursor + 1;
		//	}
		//	fader->FadeIn(1.0f);
		//	SceneManager::ChangeScene("PLAY");
		//}
		if (rectSelect == RECT_LEFT)
		{
			PlayScene::SelectedStage = 1;
		}
		else
		{
			PlayScene::SelectedStage = 2;
		}

		fader->FadeIn(1.0f);
		SceneManager::ChangeScene("PLAY");
	}

	// --- 点滅制御 ---
	blinkTimer++;
	if (blinkTimer > 30)
	{
		blinkOn = !blinkOn;
		blinkTimer = 0;
	}


	// ESCで終了
	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}
}


void SelectStage::Draw()
{

	DrawGraph(0, 0, backgroundImage, FALSE);

	// 四角
	DrawRoundRect(320, 300, 800, 780, 5, 5, GetColor(0, 200, 0), TRUE);
	DrawRoundRect(1120, 300, 1600, 780, 5, 5, GetColor(255, 0, 0), TRUE);

	// 点滅する枠
	if (blinkOn)
	{
		if (rectSelect == RECT_LEFT)
		{
			DrawRoundRect(315, 295, 805, 785, 8, 8, GetColor(0, 0, 0), FALSE);
		}
		else
		{
			DrawRoundRect(1115, 295, 1605, 785, 8, 8, GetColor(0, 0, 0), FALSE);
		}
	}

	//DrawFormatString(0, 0, GetColor(0, 0, 0), "選択番号＝%d", cursor);
	//int colorNormal = GetColor(0, 0, 0);
	//int colorSelect = GetColor(255, 69, 0);

	//SetFontSize(36);

	//int lineHeight = 50;                 // 1行ごとの間隔
	//int totalHeight = lineHeight * stageNames.size();

	//// 画面中央に全体を配置するための開始位置
	//int startY = (Screen::HEIGHT - totalHeight) / 2;

	//for (int i = 0; i < stageNames.size(); i++)
	//{
	//	int color = stageEnabled[i] ? (i == cursor ? colorSelect : colorNormal) : GetColor(255, 255, 255);  // 無効なステージは背景色と同化させてます

	//	// 各行の幅を取得して横中央に配置
	//	int textWidth = GetDrawStringWidth(stageNames[i].c_str(), -1);
	//	int x = (Screen::WIDTH - textWidth) / 2;

	//	int y = startY + i * lineHeight;

	//	DrawString(x, y, stageNames[i].c_str(), color);
	//}
	// 
	// 文字
	SetFontSize(36);
	DrawString(450, 500, "EASY", GetColor(0, 0, 0));
	DrawString(1250, 500, "HARD", GetColor(0, 0, 0));

	// 説明文も中央
	const char* msg1 = "タイトルへ戻る  Push to [T]";
	int w = GetDrawStringWidth(msg1, -1);
	DrawString((Screen::WIDTH - w) / 2, Screen::HEIGHT - 80, msg1, GetColor(0, 0, 0));

	const char* msg2 = "挑戦したいステージのキーを押してください";
	int w2 = GetDrawStringWidth(msg2, -1);
	DrawString((Screen::WIDTH - w2) / 2, Screen::HEIGHT - 900, msg2, GetColor(0, 0, 0));

	SetFontSize(100);
	const char* msg3 = "ステージ選択";
	int w3 = GetDrawStringWidth(msg3, -1);
	DrawString((Screen::WIDTH - w3) / 2, Screen::HEIGHT - 1050, msg3, GetColor(255, 255, 0));
}