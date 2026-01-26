#include "SelectStage.h"

#include <algorithm>
using std::max;
using std::min;

#include <cmath>
#include <cstdlib>

#include "../Library/SceneManager.h"
#include "PlayScene.h"

static const TCHAR* kWallPath = TEXT("data/image/kabe.png");

static int CreateJPFont(const TCHAR* name, int size, int thick)
{
	int h = CreateFontToHandle(name, size, thick, DX_FONTTYPE_ANTIALIASING_8X8);
	return h;
}

SelectStage::SelectStage()
{
	wallImg_ = LoadGraph(kWallPath);

	fontTitle_ = CreateJPFont(TEXT("HGS創英角ﾎﾟｯﾌﾟ体"), 84, 4);
	if (fontTitle_ < 0) fontTitle_ = CreateJPFont(TEXT("Meiryo UI"), 84, 4);
	if (fontTitle_ < 0) fontTitle_ = CreateJPFont(TEXT("MS ゴシック"), 84, 3);
	if (fontTitle_ < 0) fontTitle_ = CreateJPFont(TEXT("Arial Black"), 84, 4);

	fontSub_ = CreateJPFont(TEXT("HGS創英角ﾎﾟｯﾌﾟ体"), 34, 2);
	if (fontSub_ < 0) fontSub_ = CreateJPFont(TEXT("Meiryo UI"), 34, 2);
	if (fontSub_ < 0) fontSub_ = CreateJPFont(TEXT("MS ゴシック"), 34, 2);
	if (fontSub_ < 0) fontSub_ = CreateJPFont(TEXT("Arial"), 34, 2);

	fontCard_ = CreateJPFont(TEXT("BIZ UDP明朝 Medium"), 40, 2);
	if (fontCard_ < 0) fontCard_ = CreateJPFont(TEXT("Meiryo UI"), 34, 2);
	if (fontCard_ < 0) fontCard_ = CreateJPFont(TEXT("MS ゴシック"), 34, 2);
	if (fontCard_ < 0) fontCard_ = CreateJPFont(TEXT("Arial Black"), 34, 2);

	fontHint_ = CreateJPFont(TEXT("メイリオ"), 28, 2);
	if (fontHint_ < 0) fontHint_ = CreateJPFont(TEXT("Meiryo UI"), 28, 2);
	if (fontHint_ < 0) fontHint_ = CreateJPFont(TEXT("MS ゴシック"), 28, 2);
	if (fontHint_ < 0) fontHint_ = CreateJPFont(TEXT("Arial"), 28, 2);

	// Sparkles init
	int sw = 1280, sh = 720;
	GetDrawScreenSize(&sw, &sh);
	for (int i = 0; i < kSparkleCount; ++i)
	{
		sp_[i].x = (float)(GetRand(sw));
		sp_[i].y = (float)(GetRand(sh));
		sp_[i].v = 0.3f + (float)GetRand(100) / 200.0f;  // 0.3..0.8
		sp_[i].s = 2.0f + (float)GetRand(18);           // 2..20
		sp_[i].kind = GetRand(2);                       // 0..2
	}
}

SelectStage::~SelectStage()
{
	if (wallImg_ >= 0) DeleteGraph(wallImg_);
	if (fontTitle_ >= 0) DeleteFontToHandle(fontTitle_);
	if (fontSub_ >= 0) DeleteFontToHandle(fontSub_);
	if (fontCard_ >= 0) DeleteFontToHandle(fontCard_);
	if (fontHint_ >= 0) DeleteFontToHandle(fontHint_);
}

void SelectStage::Decide(int stageId)
{
	PlayScene::SelectedStage = stageId; // 1=Easy, 2=Hard
	deciding_ = true;
	fade_ = 0.0f;
}

void SelectStage::Update()
{
	++blink_;

	// Background slow scroll
	wallScroll_ += 0.6f;
	if (wallScroll_ > 100000.0f) wallScroll_ = 0.0f;

	// Return title
	if (CheckHitKey(KEY_INPUT_SPACE) || CheckHitKey(KEY_INPUT_T))
	{
		SceneManager::ChangeScene("TITLE");
		return;
	}

	if (!deciding_)
	{
		// Direct pick like the design (E / D)
		if (CheckHitKey(KEY_INPUT_E)) { selected_ = 0; Decide(1); return; }
		if (CheckHitKey(KEY_INPUT_H)) { selected_ = 1; Decide(2); return; }

		// Move selection
		if (CheckHitKey(KEY_INPUT_LEFT) || CheckHitKey(KEY_INPUT_A)) selected_ = 0;
		if (CheckHitKey(KEY_INPUT_RIGHT) || CheckHitKey(KEY_INPUT_F)) selected_ = 1;

		// Confirm
		if (CheckHitKey(KEY_INPUT_RETURN) || CheckHitKey(KEY_INPUT_Z))
		{
			Decide(selected_ == 0 ? 1 : 2);
			return;
		}

		if (CheckHitKey(KEY_INPUT_ESCAPE))  SceneManager::Exit();
	}
	else
	{
		fade_ += 1.0f / 30.0f; // ~0.5s
		if (fade_ >= 1.0f)
		{
			SceneManager::ChangeScene("PLAY");
			return;
		}
	}
}

void SelectStage::DrawTiledWall(int sw, int sh) const
{
	if (wallImg_ < 0)
	{
		DrawBox(0, 0, sw, sh, GetColor(12, 12, 16), TRUE);
		return;
	}

	int iw = 0, ih = 0;
	GetGraphSize(wallImg_, &iw, &ih);
	if (iw <= 0 || ih <= 0)
	{
		DrawBox(0, 0, sw, sh, GetColor(12, 12, 16), TRUE);
		return;
	}

	const int offX = (int)wallScroll_ % iw;

	for (int y = 0; y < sh; y += ih)
	{
		for (int x = -iw; x < sw + iw; x += iw)
		{
			DrawGraph(x - offX, y, wallImg_, TRUE);
		}
	}

	// Darken a bit for readability
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
	DrawBox(0, 0, sw, sh, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SelectStage::DrawVignette(int sw, int sh) const
{
	// Simple vignette using alpha boxes
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
	DrawBox(0, 0, sw, 60, GetColor(0, 0, 0), TRUE);
	DrawBox(0, sh - 60, sw, sh, GetColor(0, 0, 0), TRUE);
	DrawBox(0, 0, 60, sh, GetColor(0, 0, 0), TRUE);
	DrawBox(sw - 60, 0, sw, sh, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SelectStage::DrawSparkles(int sw, int sh) const
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
	const int c1 = GetColor(255, 220, 120);
	const int c2 = GetColor(255, 255, 255);

	for (int i = 0; i < kSparkleCount; ++i)
	{
		float t = (float)((blink_ + i * 7) % 120) / 120.0f;
		int a = 60 + (int)(120.0f * std::fabs(std::sin(t * 3.14159f * 2.0f)));
		a = min(220, max(40, a));
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, a);

		int x = (int)sp_[i].x;
		int y = (int)sp_[i].y;
		int s = (int)sp_[i].s;

		if (sp_[i].kind == 0)
		{
			DrawLine(x - s, y, x + s, y, c1);
			DrawLine(x, y - s, x, y + s, c1);
		}
		else if (sp_[i].kind == 1)
		{
			DrawCircle(x, y, max(1, s / 3), c2, TRUE);
		}
		else
		{
			DrawTriangle(x, y - s, x - s, y + s, x + s, y + s, c1, TRUE);
		}
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SelectStage::Draw()
{
	int sw = 0, sh = 0;
	GetDrawScreenSize(&sw, &sh);

	DrawTiledWall(sw, sh);

	// Base layout frame (same as your current baseline)
	const int marginX = max(60, sw / 16);
	const int marginY = max(50, sh / 14);
	const int frameX = marginX;
	const int frameY = marginY;
	const int frameW = sw - marginX * 2;
	const int frameH = sh - marginY * 2;

	// Drop shadow for the whole panel
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
	DrawBox(frameX + 10, frameY + 14, frameX + frameW + 10, frameY + frameH + 14, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	const int frameLine = GetColor(80, 130, 240);
	const int frameFill = GetColor(245, 245, 245);

	//DrawBox(frameX, frameY, frameX + frameW, frameY + frameH, frameFill, TRUE);
	DrawBox(frameX, frameY, frameX + frameW, frameY + frameH, frameLine, FALSE);

	// Header ribbon decoration
	//SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
	//DrawBox(frameX, frameY, frameX + frameW, frameY + 170, GetColor(255, 255, 255), TRUE);
	//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// Sparkles on top area
	DrawSparkles(sw, sh);

	// Title
	const TCHAR* title = TEXT("ステージ選択");
	int tw = GetDrawStringWidthToHandle(title, (int)_tcslen(title), fontTitle_);
	DrawStringToHandle(frameX + frameW / 2 - tw / 2, frameY + 30, title, GetColor(255, 180, 0), fontTitle_);

	// Subtitle
	const TCHAR* sub = TEXT("挑戦したいステージのキーを押して下さい");
	int subw = GetDrawStringWidthToHandle(sub, (int)_tcslen(sub), fontSub_);
	DrawStringToHandle(frameX + frameW / 2 - subw / 2, frameY + 130, sub, GetColor(200, 200, 200), fontSub_);


	const int innerTop = frameY + 220;
	const int innerBottom = frameY + frameH - 140;

	const int gap = max(50, frameW / 12);
	const int cardW = min(640, (frameW - gap) / 2);
	const int cardH = min(460, innerBottom - innerTop);

	const int leftX = frameX + (frameW - (cardW * 2 + gap)) / 2;
	const int rightX = leftX + cardW + gap;
	const int cardY = innerTop + (innerBottom - innerTop - cardH) / 2;

	const int easyFill = GetColor(110, 170, 70);
	const int hardFill = GetColor(220, 40, 40);
	const int cardBorder = GetColor(30, 80, 160);

	// 角丸半径（大きいほど丸い）
	const int radius = 24;

	// 角丸カード描画（塗り＋枠）
	auto drawRoundCard = [&](int x, int y, int w, int h, int fillCol)
		{
			DrawRoundRect(x, y, x + w, y + h, radius, radius, fillCol, TRUE);
			DrawRoundRect(x, y, x + w, y + h, radius, radius, cardBorder, FALSE);
		};

	// Card shadow（影も角丸に）
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 110);
	DrawRoundRect(leftX + 10, cardY + 12, leftX + cardW + 10, cardY + cardH + 12, radius, radius, GetColor(0, 0, 0), TRUE);
	DrawRoundRect(rightX + 10, cardY + 12, rightX + cardW + 10, cardY + cardH + 12, radius, radius, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// Card body
	drawRoundCard(leftX, cardY, cardW, cardH, easyFill);
	drawRoundCard(rightX, cardY, cardW, cardH, hardFill);

	// Selected highlight
	const bool blinkOn = ((blink_ / 18) % 2) == 0;
	const int hiCol = GetColor(255, 235, 150);

	if (blinkOn)
	{
		const int bx = (selected_ == 0) ? leftX : rightX;
		for (int i = 0; i < 8; ++i)
		{
			// 枠を外側に広げるぶん、角丸半径も少しだけ増やす
			DrawRoundRect(bx - i, cardY - i, bx + cardW + i, cardY + cardH + i,
				radius + i, radius + i, hiCol, FALSE);
		}

		// Shine stripe
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
		DrawBox(bx + 18, cardY + 18, bx + cardW - 18, cardY + 54, GetColor(255, 255, 255), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}


	auto drawCenterBlock = [&](int cx, int cardTopY, int cardH,
		const TCHAR* const* lines, int n,
		int fontHandle, int lineGap,
		int offsetY)
		{
			// 文字ブロック全体の高さ
			const int blockH = (n - 1) * lineGap + GetFontSizeToHandle(fontHandle);

			int y = cardTopY + (cardH - blockH) / 2 + offsetY;

			for (int i = 0; i < n; ++i)
			{
				const TCHAR* s = lines[i];
				int w = GetDrawStringWidthToHandle(s, (int)_tcslen(s), fontHandle);
				DrawStringToHandle(cx - w / 2, y, s, GetColor(255, 255, 255), fontHandle);
				y += lineGap;
			}
		};


	const TCHAR* easyLines[] = {
		TEXT("易しいチュートリアル"),
		TEXT("すぐイライラしちゃう人に"),
		TEXT("おすすめ"),
		TEXT("残機数 5"),
		TEXT("Push to [E]")
	};
	const TCHAR* hardLines[] = {
		TEXT("難しい"),
		TEXT("たくさん死にたい人におすすめ"),
		TEXT("残機数 3"),
		TEXT("Push to [H]")
	};

	drawCenterBlock(leftX + cardW / 2, cardY, cardH, easyLines, 5, fontCard_, 46, -10);
	drawCenterBlock(rightX + cardW / 2, cardY, cardH, hardLines, 4, fontCard_, 50, 10);

	// Bottom hint
	const TCHAR* bottom = TEXT("タイトルへ戻る  Push to [Space]");
	int bw = GetDrawStringWidthToHandle(bottom, (int)_tcslen(bottom), fontHint_);
	DrawStringToHandle(frameX + frameW / 2 - bw / 2, frameY + frameH - 80, bottom, GetColor(188, 188, 188), fontHint_);

	DrawVignette(sw, sh);

	// Fade after decide
	if (deciding_)
	{
		int a = (int)(255.0f * min(1.0f, max(0.0f, fade_)));
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, a);
		DrawBox(0, 0, sw, sh, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}
