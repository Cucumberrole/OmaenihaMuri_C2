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
	int h = DxLib::CreateFontToHandle(name, size, thick, DX_FONTTYPE_ANTIALIASING_8X8);
	return h;
}

SelectStage::SelectStage()
{
	wallImg_ = DxLib::LoadGraph(kWallPath);

	// Japanese text may show as □ if font doesn't support it; try JP fonts in order.
	fontTitle_ = CreateJPFont(TEXT("メイリオ"), 84, 4);
	if (fontTitle_ < 0) fontTitle_ = CreateJPFont(TEXT("Meiryo UI"), 84, 4);
	if (fontTitle_ < 0) fontTitle_ = CreateJPFont(TEXT("MS ゴシック"), 84, 3);
	if (fontTitle_ < 0) fontTitle_ = CreateJPFont(TEXT("Arial Black"), 84, 4);

	fontSub_ = CreateJPFont(TEXT("メイリオ"), 34, 2);
	if (fontSub_ < 0) fontSub_ = CreateJPFont(TEXT("Meiryo UI"), 34, 2);
	if (fontSub_ < 0) fontSub_ = CreateJPFont(TEXT("MS ゴシック"), 34, 2);
	if (fontSub_ < 0) fontSub_ = CreateJPFont(TEXT("Arial"), 34, 2);

	fontCard_ = CreateJPFont(TEXT("メイリオ"), 34, 2);
	if (fontCard_ < 0) fontCard_ = CreateJPFont(TEXT("Meiryo UI"), 34, 2);
	if (fontCard_ < 0) fontCard_ = CreateJPFont(TEXT("MS ゴシック"), 34, 2);
	if (fontCard_ < 0) fontCard_ = CreateJPFont(TEXT("Arial Black"), 34, 2);

	fontHint_ = CreateJPFont(TEXT("メイリオ"), 28, 2);
	if (fontHint_ < 0) fontHint_ = CreateJPFont(TEXT("Meiryo UI"), 28, 2);
	if (fontHint_ < 0) fontHint_ = CreateJPFont(TEXT("MS ゴシック"), 28, 2);
	if (fontHint_ < 0) fontHint_ = CreateJPFont(TEXT("Arial"), 28, 2);

	// Sparkles init
	int sw = 1280, sh = 720;
	DxLib::GetDrawScreenSize(&sw, &sh);
	for (int i = 0; i < kSparkleCount; ++i)
	{
		sp_[i].x = (float)(DxLib::GetRand(sw));
		sp_[i].y = (float)(DxLib::GetRand(sh));
		sp_[i].v = 0.3f + (float)DxLib::GetRand(100) / 200.0f;  // 0.3..0.8
		sp_[i].s = 2.0f + (float)DxLib::GetRand(18);           // 2..20
		sp_[i].kind = DxLib::GetRand(2);                       // 0..2
	}
}

SelectStage::~SelectStage()
{
	if (wallImg_ >= 0) DxLib::DeleteGraph(wallImg_);
	if (fontTitle_ >= 0) DxLib::DeleteFontToHandle(fontTitle_);
	if (fontSub_ >= 0) DxLib::DeleteFontToHandle(fontSub_);
	if (fontCard_ >= 0) DxLib::DeleteFontToHandle(fontCard_);
	if (fontHint_ >= 0) DxLib::DeleteFontToHandle(fontHint_);
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
	if (DxLib::CheckHitKey(KEY_INPUT_SPACE) || DxLib::CheckHitKey(KEY_INPUT_T))
	{
		SceneManager::ChangeScene("TITLE");
		return;
	}

	if (!deciding_)
	{
		// Direct pick like the design (E / D)
		if (DxLib::CheckHitKey(KEY_INPUT_E)) { selected_ = 0; Decide(1); return; }
		if (DxLib::CheckHitKey(KEY_INPUT_D)) { selected_ = 1; Decide(2); return; }

		// Move selection
		if (DxLib::CheckHitKey(KEY_INPUT_LEFT) || DxLib::CheckHitKey(KEY_INPUT_A)) selected_ = 0;
		if (DxLib::CheckHitKey(KEY_INPUT_RIGHT) || DxLib::CheckHitKey(KEY_INPUT_F)) selected_ = 1;

		// Confirm
		if (DxLib::CheckHitKey(KEY_INPUT_RETURN) || DxLib::CheckHitKey(KEY_INPUT_Z))
		{
			Decide(selected_ == 0 ? 1 : 2);
			return;
		}
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
		DxLib::DrawBox(0, 0, sw, sh, DxLib::GetColor(12, 12, 16), TRUE);
		return;
	}

	int iw = 0, ih = 0;
	DxLib::GetGraphSize(wallImg_, &iw, &ih);
	if (iw <= 0 || ih <= 0)
	{
		DxLib::DrawBox(0, 0, sw, sh, DxLib::GetColor(12, 12, 16), TRUE);
		return;
	}

	const int offX = (int)wallScroll_ % iw;

	for (int y = 0; y < sh; y += ih)
	{
		for (int x = -iw; x < sw + iw; x += iw)
		{
			DxLib::DrawGraph(x - offX, y, wallImg_, TRUE);
		}
	}

	// Darken a bit for readability
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
	DxLib::DrawBox(0, 0, sw, sh, DxLib::GetColor(0, 0, 0), TRUE);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SelectStage::DrawVignette(int sw, int sh) const
{
	// Simple vignette using alpha boxes
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
	DxLib::DrawBox(0, 0, sw, 60, DxLib::GetColor(0, 0, 0), TRUE);
	DxLib::DrawBox(0, sh - 60, sw, sh, DxLib::GetColor(0, 0, 0), TRUE);
	DxLib::DrawBox(0, 0, 60, sh, DxLib::GetColor(0, 0, 0), TRUE);
	DxLib::DrawBox(sw - 60, 0, sw, sh, DxLib::GetColor(0, 0, 0), TRUE);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SelectStage::DrawSparkles(int sw, int sh) const
{
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
	const int c1 = DxLib::GetColor(255, 220, 120);
	const int c2 = DxLib::GetColor(255, 255, 255);

	for (int i = 0; i < kSparkleCount; ++i)
	{
		float t = (float)((blink_ + i * 7) % 120) / 120.0f;
		int a = 60 + (int)(120.0f * std::fabs(std::sin(t * 3.14159f * 2.0f)));
		a = min(220, max(40, a));
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, a);

		int x = (int)sp_[i].x;
		int y = (int)sp_[i].y;
		int s = (int)sp_[i].s;

		if (sp_[i].kind == 0)
		{
			DxLib::DrawLine(x - s, y, x + s, y, c1);
			DxLib::DrawLine(x, y - s, x, y + s, c1);
		}
		else if (sp_[i].kind == 1)
		{
			DxLib::DrawCircle(x, y, max(1, s / 3), c2, TRUE);
		}
		else
		{
			DxLib::DrawTriangle(x, y - s, x - s, y + s, x + s, y + s, c1, TRUE);
		}
	}

	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SelectStage::Draw()
{
	int sw = 0, sh = 0;
	DxLib::GetDrawScreenSize(&sw, &sh);

	DrawTiledWall(sw, sh);

	// Base layout frame (same as your current baseline)
	const int marginX = max(60, sw / 16);
	const int marginY = max(50, sh / 14);
	const int frameX = marginX;
	const int frameY = marginY;
	const int frameW = sw - marginX * 2;
	const int frameH = sh - marginY * 2;

	// Drop shadow for the whole panel
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
	DxLib::DrawBox(frameX + 10, frameY + 14, frameX + frameW + 10, frameY + frameH + 14, DxLib::GetColor(0, 0, 0), TRUE);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	const int frameLine = DxLib::GetColor(80, 130, 240);
	const int frameFill = DxLib::GetColor(245, 245, 245);

	//DxLib::DrawBox(frameX, frameY, frameX + frameW, frameY + frameH, frameFill, TRUE);
	DxLib::DrawBox(frameX, frameY, frameX + frameW, frameY + frameH, frameLine, FALSE);

	// Header ribbon decoration
	//DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
	//DxLib::DrawBox(frameX, frameY, frameX + frameW, frameY + 170, DxLib::GetColor(255, 255, 255), TRUE);
	//DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// Sparkles on top area
	DrawSparkles(sw, sh);

	// Title
	const TCHAR* title = TEXT("ステージ選択");
	int tw = DxLib::GetDrawStringWidthToHandle(title, (int)_tcslen(title), fontTitle_);
	DxLib::DrawStringToHandle(frameX + frameW / 2 - tw / 2, frameY + 30, title, DxLib::GetColor(255, 180, 0), fontTitle_);

	// Subtitle
	const TCHAR* sub = TEXT("挑戦したいステージのキーを押して下さい");
	int subw = DxLib::GetDrawStringWidthToHandle(sub, (int)_tcslen(sub), fontSub_);
	DxLib::DrawStringToHandle(frameX + frameW / 2 - subw / 2, frameY + 130, sub, DxLib::GetColor(30, 30, 30), fontSub_);

	// Cards placement (two cards, centered)
	const int innerTop = frameY + 220;
	const int innerBottom = frameY + frameH - 140;

	const int gap = max(70, frameW / 9);
	const int cardW = min(560, (frameW - gap * 3) / 2);
	const int cardH = min(420, innerBottom - innerTop);

	const int leftX = frameX + (frameW - (cardW * 2 + gap)) / 2;
	const int rightX = leftX + cardW + gap;
	const int cardY = innerTop + (innerBottom - innerTop - cardH) / 2;

	const int easyFill = DxLib::GetColor(110, 170, 70);
	const int hardFill = DxLib::GetColor(220, 40, 40);
	const int cardBorder = DxLib::GetColor(30, 80, 160);

	// Card shadow
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 110);
	DxLib::DrawBox(leftX + 10, cardY + 12, leftX + cardW + 10, cardY + cardH + 12, DxLib::GetColor(0, 0, 0), TRUE);
	DxLib::DrawBox(rightX + 10, cardY + 12, rightX + cardW + 10, cardY + cardH + 12, DxLib::GetColor(0, 0, 0), TRUE);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DxLib::DrawBox(leftX, cardY, leftX + cardW, cardY + cardH, easyFill, TRUE);
	DxLib::DrawBox(leftX, cardY, leftX + cardW, cardY + cardH, cardBorder, FALSE);

	DxLib::DrawBox(rightX, cardY, rightX + cardW, cardY + cardH, hardFill, TRUE);
	DxLib::DrawBox(rightX, cardY, rightX + cardW, cardY + cardH, cardBorder, FALSE);

	// Selected highlight (glow + shine)
	const bool blinkOn = ((blink_ / 18) % 2) == 0;
	const int hiCol = DxLib::GetColor(255, 235, 150);

	if (blinkOn)
	{
		int bx = (selected_ == 0) ? leftX : rightX;
		for (int i = 0; i < 8; ++i)
		{
			DxLib::DrawBox(bx - i, cardY - i, bx + cardW + i, cardY + cardH + i, hiCol, FALSE);
		}

		// Shine stripe
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
		DxLib::DrawBox(bx + 18, cardY + 18, bx + cardW - 18, cardY + 54, DxLib::GetColor(255, 255, 255), TRUE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	auto drawCenterLines = [&](int cx, int topY, const TCHAR* const* lines, int n, int lineGap)
		{
			int y = topY;
			for (int i = 0; i < n; ++i)
			{
				const TCHAR* s = lines[i];
				int w = DxLib::GetDrawStringWidthToHandle(s, (int)_tcslen(s), fontCard_);
				DxLib::DrawStringToHandle(cx - w / 2, y, s, DxLib::GetColor(255, 255, 255), fontCard_);
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
		TEXT("Push to [D]")
	};

	drawCenterLines(leftX + cardW / 2, cardY + 90, easyLines, 5, 46);
	drawCenterLines(rightX + cardW / 2, cardY + 110, hardLines, 4, 50);

	// Bottom hint
	const TCHAR* bottom = TEXT("タイトルへ戻る  Push to [Space]");
	int bw = DxLib::GetDrawStringWidthToHandle(bottom, (int)_tcslen(bottom), fontHint_);
	DxLib::DrawStringToHandle(frameX + frameW / 2 - bw / 2, frameY + frameH - 80, bottom, DxLib::GetColor(30, 30, 30), fontHint_);

	DrawVignette(sw, sh);

	// Fade after decide
	if (deciding_)
	{
		int a = (int)(255.0f * min(1.0f, max(0.0f, fade_)));
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, a);
		DxLib::DrawBox(0, 0, sw, sh, DxLib::GetColor(0, 0, 0), TRUE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}
