#include "SelectStage.h"
#include "PlayScene.h"
#include "Screen.h"
#include "../Library/SceneManager.h"
#include "../Library/Trigger.h"
#include <DxLib.h>


SelectStage::SelectStage()
{
	backgroundImage = LoadGraph("data/image/stageselect.png");

	// 表示するステージ名
	stageNames = { "STAGE 1", "STAGE 2", "STAGE 3" };

	cursor = 0;
}

SelectStage::~SelectStage()
{
}

void SelectStage::Update()
{
	// --- 上下でカーソル移動 ---
	if (KeyTrigger::CheckTrigger(KEY_INPUT_UP)) {
		cursor--;
		if (cursor < 0) cursor = stageNames.size() - 1;
	}
	if (KeyTrigger::CheckTrigger(KEY_INPUT_DOWN)) {
		cursor++;
		if (cursor >= stageNames.size()) cursor = 0;
	}

	// --- 決定でステージへ ---
	if (KeyTrigger::CheckTrigger(KEY_INPUT_RETURN))
	{
		PlayScene::SelectedStage = cursor + 1;
		SceneManager::ChangeScene("PLAY");
	}

	// ESCで終了
	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}
}

void SelectStage::Draw()
{
	// 背景画像
	DrawGraph(0, 0, backgroundImage, FALSE);

	int colorNormal = GetColor(255, 255, 255);
	int colorSelect = GetColor(255, 255, 0);

	SetFontSize(36);

	for (int i = 0; i < stageNames.size(); i++)
	{
		int color = (i == cursor) ? colorSelect : colorNormal;

		DrawString(200, 200 + i * 50, stageNames[i].c_str(), color);
	}

	int w = GetDrawStringWidth("↑↓で選択 / ENTERで決定", -1);
	DrawString((Screen::WIDTH - w) / 2, Screen::HEIGHT - 80,
		"↑↓で選択 / ENTERで決定", GetColor(200, 200, 200));
}
