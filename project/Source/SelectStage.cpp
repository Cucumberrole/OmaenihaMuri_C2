#include "SelectStage.h"
#include "PlayScene.h"
#include "../Library/SceneManager.h"
#include "../Library/Trigger.h"
#include <DxLib.h>
#include <algorithm>

SelectStage::SelectStage()
{
	options.clear();

	Option easy;
	easy.title = "易しいチュートリアル";
	easy.sub1 = "すぐイライラしちゃう人に";
	easy.sub2 = "おすすめ";
	easy.lives = 5;
	easy.stageId = 1;                 // stage01.csv
	easy.hotKey = KEY_INPUT_E;       // E
	easy.boxColor = GetColor(80, 160, 80);
	options.push_back(easy);

	Option hard;
	hard.title = "難しい";
	hard.sub1 = "たくさん死にたい人におすすめ";
	hard.sub2 = "";
	hard.lives = 3;
	hard.stageId = 2;                 // stage02.csv
	hard.hotKey = KEY_INPUT_D;       // D
	hard.boxColor = GetColor(220, 30, 30);
	options.push_back(hard);

	cursor = 0;
}

SelectStage::~SelectStage()
{
}

void SelectStage::Update()
{
	// タイトルに戻る
	if (KeyTrigger::CheckTrigger(KEY_INPUT_SPACE))
	{
		SceneManager::ChangeScene("TITLE");
		return;
	}

	// 左右で移動（任意）
	if (KeyTrigger::CheckTrigger(KEY_INPUT_LEFT))
	{
		cursor--;
		if (cursor < 0) cursor = (int)options.size() - 1;
	}
	if (KeyTrigger::CheckTrigger(KEY_INPUT_RIGHT))
	{
		cursor++;
		if (cursor >= (int)options.size()) cursor = 0;
	}

	// ホットキーで即決定
	for (int i = 0; i < (int)options.size(); ++i)
	{
		if (KeyTrigger::CheckTrigger(options[i].hotKey))
		{
			cursor = i;
			PlayScene::SelectedStage = options[i].stageId;
			// 難易度別残機をここで渡すなら、PlayScene側に static を用意して代入
			SceneManager::ChangeScene("PLAY");
			return;
		}
	}

	// Enterでも決定
	if (KeyTrigger::CheckTrigger(KEY_INPUT_RETURN))
	{
		PlayScene::SelectedStage = options[cursor].stageId;
		SceneManager::ChangeScene("PLAY");
		return;
	}

	if (CheckHitKey(KEY_INPUT_ESCAPE))
	{
		SceneManager::Exit();
	}
}

void SelectStage::Draw()
{
	ClearDrawScreen();

	int scrW = 0, scrH = 0;
	GetDrawScreenSize(&scrW, &scrH);

	// 背景
	DrawBox(0, 0, scrW, scrH, GetColor(255, 255, 255), TRUE);

	// 画面サイズに応じて余白を自動調整
	int margin = max(30, min(scrW, scrH) / 20);
	int frameL = margin;
	int frameT = margin;
	int frameR = scrW - margin;
	int frameB = scrH - margin;

	DrawBox(frameL, frameT, frameR, frameB, GetColor(255, 255, 255), TRUE);

	// タイトル
	SetFontSize(max(40, scrH / 12));
	int titleColor = GetColor(255, 180, 0);
	const char* title = "ステージ選択";
	int titleW = GetDrawStringWidth(title, -1);
	DrawString((scrW - titleW) / 2, frameT + margin / 2, title, titleColor);

	// サブ
	SetFontSize(max(18, scrH / 40));
	int textColor = GetColor(0, 0, 0);
	const char* sub = "挑戦したいステージのキーを押して下さい";
	int subW = GetDrawStringWidth(sub, -1);
	DrawString((scrW - subW) / 2, frameT + margin / 2 + max(60, scrH / 10), sub, textColor);

	// ボックスレイアウト（2列）
	int cols = 2;
	int gapX = max(40, scrW / 12);
	int gapY = max(30, scrH / 18);

	// フレーム内の使用可能領域
	int areaL = frameL + margin;
	int areaR = frameR - margin;
	int areaT = frameT + margin + max(140, scrH / 5);     // タイトル領域ぶん下げる
	int areaB = frameB - margin - max(60, scrH / 12);     // 下の説明ぶん確保

	int areaW = areaR - areaL;
	int areaH = areaB - areaT;

	int boxW = (areaW - gapX) / 2;
	int boxH = min((int)(areaH * 0.75f), boxW); // 縦を詰めすぎない＆正方形寄り

	// 全体を縦中央寄せしたい場合は rows を計算して startY を調整
	int n = (int)options.size();
	int rows = (n + cols - 1) / cols;
	int totalH = rows * boxH + (rows - 1) * gapY;
	int startY = areaT + max(0, (areaH - totalH) / 2);
	int startX = areaL + max(0, (areaW - (boxW * cols + gapX * (cols - 1))) / 2);

	auto drawCenter = [&](int bx, int by, int bw, const std::string& s, int yy, int fontSize, int col)
		{
			if (s.empty()) return;
			SetFontSize(fontSize);
			int w = GetDrawStringWidth(s.c_str(), -1);
			DrawString(bx + (bw - w) / 2, yy, s.c_str(), col);
		};

	for (int i = 0; i < n; ++i)
	{
		int r = i / cols;
		int c = i % cols;

		int bx = startX + c * (boxW + gapX);
		int by = startY + r * (boxH + gapY);

		// 塗り
		DrawBox(bx, by, bx + boxW, by + boxH, options[i].boxColor, TRUE);

		// 枠（選択中は太く）
		int border = (i == cursor) ? 6 : 2;
		int borderColor = (i == cursor) ? GetColor(255, 255, 255) : GetColor(0, 0, 0);
		for (int t = 0; t < border; ++t)
		{
			DrawBox(bx - t, by - t, bx + boxW + t, by + boxH + t, borderColor, FALSE);
		}

		int white = GetColor(255, 255, 255);

		int y = by + boxH / 4;
		drawCenter(bx, by, boxW, options[i].title, y, max(26, scrH / 28), white); y += max(40, scrH / 18);
		drawCenter(bx, by, boxW, options[i].sub1, y, max(20, scrH / 36), white); y += max(32, scrH / 24);
		drawCenter(bx, by, boxW, options[i].sub2, y, max(20, scrH / 36), white); y += max(44, scrH / 18);

		// 残機
		{
			char buf[64];
			sprintf_s(buf, "残機数 %d", options[i].lives);
			drawCenter(bx, by, boxW, buf, y, max(20, scrH / 36), white);
			y += max(44, scrH / 18);
		}

		// Push
		{
			std::string keyStr = "Push to[";
			if (options[i].hotKey == KEY_INPUT_E) keyStr += "E";
			else if (options[i].hotKey == KEY_INPUT_D) keyStr += "D";
			else keyStr += "?";
			keyStr += "]";
			drawCenter(bx, by, boxW, keyStr, y, max(20, scrH / 36), white);
		}
	}

	// 下の説明
	SetFontSize(max(18, scrH / 42));
	const char* msg = "タイトルへ戻る  Push to[Space]";
	int msgW = GetDrawStringWidth(msg, -1);
	DrawString((scrW - msgW) / 2, frameB - margin - max(30, scrH / 30), msg, textColor);
}