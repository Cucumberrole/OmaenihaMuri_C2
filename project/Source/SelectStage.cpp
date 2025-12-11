#include "SelectStage.h"
#include "PlayScene.h"
#include "Screen.h"
#include "Fader.h"
#include "../Library/SceneManager.h"
#include "../Library/Trigger.h"
#include <DxLib.h>


SelectStage::SelectStage()
{
	backgroundImage = LoadGraph("data/image/title.png");

	// 表示するステージ名
	stageNames = { "STAGE 1", "STAGE 2", "STAGE 3", "DEBUG STAGE" };
	stageEnabled = { true,true,false,false };

	cursor = 0;
}

SelectStage::~SelectStage()
{
}

void SelectStage::Update()
{
<<<<<<< HEAD
	Fader* fader = FindGameObject<Fader>();
	// --- �㉺�ŃJ�[�\���ړ� ---
	// --- ��ړ� ---
=======
	// --- 上下でカーソル移動 ---
	// --- 上移動 ---
>>>>>>> 94ba39019be8c19bf43fb01ed716a2f4671335fc
	if (KeyTrigger::CheckTrigger(KEY_INPUT_UP)) {
		do {
			cursor--;
			if (cursor < 0) cursor = stageNames.size() - 1;
		} while (!stageEnabled[cursor]);  // 無効なら飛ばす
	}

	// --- 下移動 ---
	if (KeyTrigger::CheckTrigger(KEY_INPUT_DOWN)) {
		do {
			cursor++;
			if (cursor >= stageNames.size()) cursor = 0;
		} while (!stageEnabled[cursor]);  // 無効なら飛ばす
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

		if (stageEnabled[cursor])    // 有効なときだけ
		{
			// デバッグステージ（cursor == 2）の時だけ、ステージナンバーを 0 に設定する
			if (cursor == 3)
			{
				PlayScene::SelectedStage = 0;
			}
			else
			{
				// STAGE 1, STAGE 2 の場合は、これまで通り cursor + 1
				PlayScene::SelectedStage = cursor + 1;
			}
<<<<<<< HEAD
			fader->FadeIn(3.0f);
=======
			PlayScene::LifeCount = 5;
	
>>>>>>> 94ba39019be8c19bf43fb01ed716a2f4671335fc
			SceneManager::ChangeScene("PLAY");
		}
	}

	// ESCで終了
	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}
}

void SelectStage::Draw()
{
	
	DrawGraph(0, 0, backgroundImage, FALSE);

<<<<<<< HEAD
	DrawBox(100, 100, 100, 100, GetColor(180, 190, 0), TRUE);
	DrawBox(400, 100, 100, 100, GetColor(255, 0, 0), TRUE);

	DrawFormatString(0, 0, GetColor(0, 0, 0), "�I��ԍ���%d", cursor);
=======
	DrawFormatString(0, 0, GetColor(0, 0, 0), "選択番号＝%d", cursor);
>>>>>>> 94ba39019be8c19bf43fb01ed716a2f4671335fc
	int colorNormal = GetColor(0, 0, 0);
	int colorSelect = GetColor(255, 69, 0);

	SetFontSize(36);

	int lineHeight = 50;                 // 1行ごとの間隔
	int totalHeight = lineHeight * stageNames.size();

	// 画面中央に全体を配置するための開始位置
	int startY = (Screen::HEIGHT - totalHeight) / 2;

	for (int i = 0; i < stageNames.size(); i++)
	{
		int color = stageEnabled[i] ? (i == cursor ? colorSelect : colorNormal) : GetColor(255, 255, 255);  // 無効なステージは背景色と同化させてます

		// 各行の幅を取得して横中央に配置
		int textWidth = GetDrawStringWidth(stageNames[i].c_str(), -1);
		int x = (Screen::WIDTH - textWidth) / 2;

		int y = startY + i * lineHeight;

		DrawString(x, y, stageNames[i].c_str(), color);
	}

	// 説明文も中央
	const char* msg = "↑↓で選択 / ENTERで決定";
	int w = GetDrawStringWidth(msg, -1);
	DrawString((Screen::WIDTH - w) / 2, Screen::HEIGHT - 80, msg, GetColor(0, 0, 0));

}