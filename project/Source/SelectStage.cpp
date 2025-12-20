#include "SelectStage.h"
#include "PlayScene.h"
#include "../Library/SceneManager.h"
#include "../Library/Trigger.h"
#include <DxLib.h>
#include <algorithm>
#include <math.h>

SelectStage::SelectStage()
{
	options.clear();

	debugUnlocked = false;
	cursor = 0;

	cursorBlinkFrame = 0;
	subTextAnimFrame = 0;

	Option easy;
	easy.title = "易しい｢チュートリアル｣";
	easy.sub1 = "すぐイライラしちゃう人向け";
	easy.sub2 = "【おすすめ】";
	easy.lives = 5;
	easy.stageId = 1;                 // Stage01
	easy.hotKey = KEY_INPUT_E;        // Eキー
	easy.boxColor = GetColor(80, 160, 80);
	options.push_back(easy);

	Option hard;
	hard.title = "難しいステージ";
	hard.sub1 = "たくさん死にたい人はこちら";
	hard.sub2 = "";
	hard.lives = 3;
	hard.stageId = 2;                 // Stage02
	hard.hotKey = KEY_INPUT_H;        // Dキー
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

	// ============================
	// デバッグステージ解放
	// ============================
	if (!debugUnlocked && KeyTrigger::CheckTrigger(KEY_INPUT_F9))
	{
		debugUnlocked = true;

		Option debug;
		debug.title = "Debugステージ";
		debug.sub1 = "開発者用テスト";
		debug.sub2 = "";
		debug.lives = 99;
		debug.stageId = 0;
		debug.hotKey = KEY_INPUT_G;
		debug.boxColor = GetColor(80, 80, 160);
		options.push_back(debug);
	}

	// 左右カーソル
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
			SceneManager::ChangeScene("PLAY");
			return;
		}
	}

	// Enterで決定
	if (KeyTrigger::CheckTrigger(KEY_INPUT_RETURN))
	{
		PlayScene::SelectedStage = options[cursor].stageId;
		SceneManager::ChangeScene("PLAY");
		return;
	}

	// ESCで終了
	if (CheckHitKey(KEY_INPUT_ESCAPE))
	{
		SceneManager::Exit();
	}

	// アニメーション用カウンタ
	cursorBlinkFrame += 0.13f; // 速さ
	if (cursorBlinkFrame > DX_PI * 2.0f)
	{
		cursorBlinkFrame -= DX_PI * 2.0f;
	}

	subTextAnimFrame += 0.08; // 速さ
	if (subTextAnimFrame > DX_PI * 2.0f)
	{
		subTextAnimFrame -= DX_PI * 2.0f;
	}
}

void SelectStage::Draw()
{
	int screenW = 0, screenH = 0;
	GetDrawScreenSize(&screenW, &screenH);

	// 念のため毎フレームクリア
	ClearDrawScreen();

	// 背景（全面）
	DrawBox(0, 0, screenW, screenH, GetColor(255, 255, 255), TRUE);

	// 外枠(青)
	int margin = max(20, (int)(min(screenW, screenH) * 0.03f)); // 3% or 20px
	int frameL = margin;
	int frameT = margin;
	int frameR = screenW - margin;
	int frameB = screenH - margin;

	int frameColor = GetColor(20, 60, 200);
	DrawBox(frameL, frameT, frameR, frameB, GetColor(255, 255, 255), TRUE);
	//DrawBox(frameL, frameT, frameR, frameB, frameColor, FALSE);

	int insideW = frameR - frameL;
	int insideH = frameB - frameT;

	// タイトル
	const char* title = "ステージ選択";
	SetFontSize(max(48, (int)(insideH * 0.08f))); // 画面に応じて
	int titleColor = GetColor(255, 180, 0);
	int titleW = GetDrawStringWidth(title, -1);
	int titleY = frameT + (int)(insideH * 0.06f);
	DrawString(frameL + (insideW - titleW) / 2, titleY, title, titleColor);

	// サブ
	const char* sub = "挑戦したいステージのキーを押して下さい";
	SetFontSize(max(20, (int)(insideH * 0.035f)));
	int subColor = GetColor(0, 0, 0);
	int subW = GetDrawStringWidth(sub, -1);
	int subY = titleY + (int)(insideH * 0.10f);
	float offset = 4.0f * sinf(subTextAnimFrame); // ±4pxくらい
	int subYAnim = subY + (int)offset;
	DrawString(frameL + (insideW - subW) / 2, subYAnim, sub, subColor);

	// 2つのボックス（左右）
	int cols = 2;

	// ボックス間隔・サイズは「内側領域」から自動計算
	int gapX = max(40, (int)(insideW * 0.06f));
	int gapY = max(30, (int)(insideH * 0.05f));

	int areaTop = subY + (int)(insideH * 0.08f);
	int areaBottom = frameB - (int)(insideH * 0.12f);
	int areaH = max(0, areaBottom - areaTop);

	int boxW = (insideW - gapX * (cols - 1)) / cols;
	int boxH = min(areaH, (int)(insideH * 0.55f)); // 高さはほどほどに

	int areaLeft = frameL + (insideW - (boxW * cols + gapX * (cols - 1))) / 2;

	for (int i = 0; i < (int)options.size(); ++i)
	{
		int r = i / cols;
		int c = i % cols;

		int bx = areaLeft + c * (boxW + gapX);
		int by = areaTop + r * (boxH + gapY);

		DrawBox(bx, by, bx + boxW, by + boxH, options[i].boxColor, TRUE);

		// 選択枠
		bool isCursor = (i == cursor);
		int border = (i == cursor) ? 6 : 2;
		int borderColor = (i == cursor) ? GetColor(0, 0, 0) : GetColor(255, 255, 255);
		if (isCursor)
		{
			float s = sinf(cursorBlinkFrame);
			int alpha = 128 + (int)(127 * s);
			if (alpha < 0)alpha = 0;
			if (alpha > 255)alpha = 255;

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		}

		for (int t = 0; t < border; ++t)
		{
			DrawBox(bx - t, by - t, bx + boxW + t, by + boxH + t, borderColor, FALSE);
		}

		if (isCursor)
		{
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}

		int textColor = GetColor(255, 255, 255);

		auto drawCenter = [&](const std::string& s, int yy, int fontSize)
			{
				if (s.empty()) return;
				SetFontSize(fontSize);
				int w = GetDrawStringWidth(s.c_str(), -1);
				DrawString(bx + (boxW - w) / 2, yy, s.c_str(), textColor);
			};

		int y = by + (int)(boxH * 0.25f);
		drawCenter(options[i].title, y, max(24, (int)(boxH * 0.09f))); y += (int)(boxH * 0.13f);
		drawCenter(options[i].sub1, y, max(18, (int)(boxH * 0.07f))); y += (int)(boxH * 0.10f);
		drawCenter(options[i].sub2, y, max(18, (int)(boxH * 0.07f))); y += (int)(boxH * 0.14f);

		{
			char buf[64];
			sprintf_s(buf, "残機数 %d", options[i].lives);
			drawCenter(buf, y, max(18, (int)(boxH * 0.07f)));
			y += (int)(boxH * 0.12f);
		}

		{
			std::string keyStr = "Push to[";
			if (options[i].hotKey == KEY_INPUT_E) keyStr += "E";
			else if (options[i].hotKey == KEY_INPUT_H) keyStr += "H";
			else keyStr += "?";
			keyStr += "]";
			drawCenter(keyStr, y, max(18, (int)(boxH * 0.07f)));
		}
	}

	// 下の説明
	const char* msg = "タイトルへ戻る Push to[Space]";
	SetFontSize(max(18, (int)(insideH * 0.03f)));
	int msgW = GetDrawStringWidth(msg, -1);
	int msgY = frameB - (int)(insideH * 0.07f);
	DrawString(frameL + (insideW - msgW) / 2, msgY, msg, subColor);
}
