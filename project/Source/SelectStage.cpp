#include "GameConfig.h"
#include "SelectStage.h"
#include "SoundCache.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <tuple>

#include <DxLib.h>

#include "../Library/SceneManager.h"
#include "PlayScene.h"

static const TCHAR* kWallPath = TEXT("data/image/kabe.png");

static int CreateJPFont(const TCHAR* name, int size, int thick)
{
	int h = CreateFontToHandle(name, size, thick, DX_FONTTYPE_ANTIALIASING_8X8);
	return h;
}

static inline float Clamp01(float v) { return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); }

// 0..1 の滑らかな補間
static inline float SmoothStep(float a, float b, float x)
{
	float t = Clamp01((x - a) / (b - a));
	return t * t * (3.f - 2.f * t);
}

// ビネット生成（srcW/srcH は生成解像度。target は最終表示サイズ）
static int BuildVignetteGraph(int srcW, int srcH, int targetW, int targetH,
	float inner = 0.55f, float outer = 1.05f,
	float power = 2.2f, int maxAlpha = 190,
	float centerYBias = -0.06f)
{
	int si = MakeARGB8ColorSoftImage(srcW, srcH);
	if (si < 0) return -1;

	const float cx = (srcW - 1) * 0.5f;
	const float cy = (srcH - 1) * 0.5f;

	const float aspect = (targetH > 0) ? (float)targetW / (float)targetH : 1.0f;
	const float invRx = 1.0f / cx;
	const float invRy = 1.0f / cy;

	for (int y = 0; y < srcH; ++y)
	{
		for (int x = 0; x < srcW; ++x)
		{
			float nx = (x - cx) * invRx;              // -1..1
			float ny = (y - cy) * invRy;              // -1..1

			ny -= centerYBias;
			nx *= (1.0f / aspect);

			float d = std::sqrt(nx * nx + ny * ny);   // 0..~1.4
			float t = SmoothStep(inner, outer, d);
			t = std::pow(t, power);

			int a = (int)(maxAlpha * t);
			if (a < 0) a = 0;
			if (a > 255) a = 255;

			DrawPixelSoftImage(si, x, y, 0, 0, 0, a);
		}
	}

	int g = CreateGraphFromSoftImage(si);
	DeleteSoftImage(si);
	return g;
}

SelectStage::SelectStage()
{
	wallImg_ = LoadGraph(kWallPath);

	fontTitle_ = CreateJPFont(TEXT("HGS創英角ﾎﾟｯﾌﾟ体"), 130, 4);
	if (fontTitle_ < 0) fontTitle_ = CreateJPFont(TEXT("Meiryo UI"), 84, 4);
	if (fontTitle_ < 0) fontTitle_ = CreateJPFont(TEXT("MS ゴシック"), 84, 3);
	if (fontTitle_ < 0) fontTitle_ = CreateJPFont(TEXT("Arial Black"), 84, 4);

	fontSub_ = CreateJPFont(TEXT("HGS創英角ﾎﾟｯﾌﾟ体"), 38, 2);
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

	// 1/2解像度で作る
	vignetteTargetW_ = sw;
	vignetteTargetH_ = sh;
	vignetteSrcW_ = max(320, sw / 2);
	vignetteSrcH_ = max(180, sh / 2);
	vignetteImg_ = BuildVignetteGraph(vignetteSrcW_, vignetteSrcH_, sw, sh);

	introStartMs_ = GetNowCount();
	introT_ = 0.0f;
	introDone_ = false;

	SelectBGM = SoundCache::Get("data/BGM/StageSelect.mp3");
	PlaySoundMem(SelectBGM, DX_PLAYTYPE_LOOP);

	SelectSE = SoundCache::Get("data/BGM/cursorSE.mp3");
	DecisionSE = SoundCache::Get("data/BGM/Decision.mp3");

	prevSelected_ = selected_;
}

SelectStage::~SelectStage()
{
	if (wallImg_ >= 0) DeleteGraph(wallImg_);
	if (fontTitle_ >= 0) DeleteFontToHandle(fontTitle_);
	if (fontSub_ >= 0) DeleteFontToHandle(fontSub_);
	if (fontCard_ >= 0) DeleteFontToHandle(fontCard_);
	if (fontHint_ >= 0) DeleteFontToHandle(fontHint_);
	if (vignetteImg_ >= 0) DeleteGraph(vignetteImg_);

	StopSoundMem(SelectBGM);
	StopSoundMem(SelectSE);
	StopSoundMem(DecisionSE);
}

void SelectStage::Decide(int stageId)
{
	//  stageId = 1/2/3 を PlayScene/Field に渡す
	PlayScene::SelectedStage = stageId;

	// ここは好みでOK（例：Stage1=Easy, Stage2=Hard, Stage3=Easy∞）
	if (stageId == 1)
	{
		SelectedDifficulty() = Difficulty::Easy;
		MaxLives() = 5;
	}
	else if (stageId == 2)
	{
		SelectedDifficulty() = Difficulty::Hard;
		MaxLives() = 3;
	}
	else // stageId == 3
	{
		SelectedDifficulty() = Difficulty::Easy;
		MaxLives() = 1; // ライフ値設定
	}

	deciding_ = true;
	fade_ = 0.0f;
}

void SelectStage::DecideDebug()
{
	PlayScene::SelectedStage = 3;
	SelectedDifficulty() = Difficulty::Easy;
	MaxLives() = 999;

	deciding_ = true;
	fade_ = 0.0f;
}

void SelectStage::Update()
{
	// == キー入力の更新 ==
	std::memcpy(keyPrev_, keyNow_, sizeof(keyNow_));
	GetHitKeyStateAll(keyNow_);

	// == 遷移アニメーション ==
	{
		const float introDurationSec = 0.25f;
		const int now = GetNowCount();
		const float elapsedSec = (now - introStartMs_) / 1000.0f;

		introElapsedSec_ = elapsedSec;  // カードの順番表示
		introT_ = (introDurationSec <= 0.0f) ? 1.0f : (elapsedSec / introDurationSec);
		if (introT_ >= 1.0f) { introT_ = 1.0f; introDone_ = true; }

		// 入力停止
		if (!introDone_)
		{
			blink_++;
			return;
		}
	}

	++blink_;

	// 背景のスクロール
	wallScroll_ += 0.6f;
	if (wallScroll_ > 100000.0f) wallScroll_ = 0.0f;

	// 戻る
	if (CheckHitKey(KEY_INPUT_SPACE) || CheckHitKey(KEY_INPUT_T))
	{
		SceneManager::ChangeScene("TITLE");
		return;
	}

	if (!deciding_)
	{
		// 隠し：デバッグ
		if (KeyDown(KEY_INPUT_F3))
		{
			DecideDebug();
			return;
		}

		// 直接選択（任意：便利）
		if (KeyDown(KEY_INPUT_1)) { selected_ = 0; PlaySoundMem(DecisionSE, DX_PLAYTYPE_BACK); Decide(1); return; }
		if (KeyDown(KEY_INPUT_2)) { selected_ = 1; PlaySoundMem(DecisionSE, DX_PLAYTYPE_BACK); Decide(2); return; }
		if (KeyDown(KEY_INPUT_3)) { selected_ = 2; PlaySoundMem(DecisionSE, DX_PLAYTYPE_BACK); Decide(3); return; }

		// AD / 矢印で選択（3択ループ）
		if (KeyDown(KEY_INPUT_LEFT) || KeyDown(KEY_INPUT_A))
		{
			selected_ = (selected_ + 3 - 1) % 3;
		}
		if (KeyDown(KEY_INPUT_RIGHT) || KeyDown(KEY_INPUT_D))
		{
			selected_ = (selected_ + 1) % 3;
		}

		if (selected_ != prevSelected_)
		{
			prevSelected_ = selected_;
			PlaySoundMem(SelectSE, DX_PLAYTYPE_BACK);
		}

		// Enterで決定（要望）
		if (KeyDown(KEY_INPUT_RETURN))
		{
			Decide(selected_ + 1);
			PlaySoundMem(DecisionSE, DX_PLAYTYPE_BACK);
			return;
		}

		if (CheckHitKey(KEY_INPUT_ESCAPE)) SceneManager::Exit();
	}
	else
	{
		fade_ += 1.0f / 30.0f; // ~0.5s
		if (fade_ >= 1.0f)
		{
			SceneManager::ChangeScene("PLAY");
			StopSoundMem(SelectBGM);
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

	// 背景、黒い四角形
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
	DrawBox(0, 0, sw, sh, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SelectStage::DrawVignette(int sw, int sh) const
{
	if (vignetteImg_ < 0) return;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 127);
	DrawExtendGraph(0, 0, sw, sh, vignetteImg_, TRUE);
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

	if (sw != vignetteTargetW_ || sh != vignetteTargetH_)
	{
		vignetteTargetW_ = sw;
		vignetteTargetH_ = sh;

		if (vignetteImg_ >= 0) { DeleteGraph(vignetteImg_); vignetteImg_ = -1; }

		vignetteSrcW_ = max(320, sw / 2);
		vignetteSrcH_ = max(180, sh / 2);
		vignetteImg_ = BuildVignetteGraph(vignetteSrcW_, vignetteSrcH_, sw, sh);
	}

	// == 遷移アニメーション（フェード） ==
	auto clamp01 = [](float t) {
		if (t < 0.0f) return 0.0f;
		if (t > 1.0f) return 1.0f;
		return t;
		};

	auto easeOutCubic = [&](float t) {
		t = clamp01(t);
		float u = 1.0f - t;
		return 1.0f - u * u * u;
		};

	const float e = easeOutCubic(introT_);
	const int enterOffsetY = (int)((1.0f - e) * 50.0f);
	const int introFadeA = (int)((1.0f - e) * 255.0f);

	DrawTiledWall(sw, sh);

	// アウトフレーム
	const int marginX = max(60, sw / 16);
	const int marginY = max(50, sh / 14);
	const int frameX = marginX;
	const int frameY = marginY + enterOffsetY;
	const int frameW = sw - marginX * 2;
	const int frameH = sh - marginY * 2;

	// Drop shadow for the whole panel
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
	DrawBox(frameX + 10, frameY + 14, frameX + frameW + 10, frameY + frameH + 14, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	const int frameLine = GetColor(80, 130, 240);
	DrawBox(frameX, frameY, frameX + frameW, frameY + frameH, frameLine, FALSE);

	DrawSparkles(sw, sh);

	// タイトル
	const TCHAR* title = TEXT("ステージ選択");
	int tw = GetDrawStringWidthToHandle(title, (int)_tcslen(title), fontTitle_);
	DrawStringToHandle(frameX + frameW / 2 - tw / 2, frameY + 30, title, GetColor(255, 180, 0), fontTitle_);

	// サブタイトル
	const TCHAR* sub = TEXT("挑戦したいステージを選んで下さい");
	int subw = GetDrawStringWidthToHandle(sub, (int)_tcslen(sub), fontSub_);
	DrawStringToHandle(frameX + frameW / 2 - subw / 2, frameY + 180, sub, GetColor(188, 188, 188), fontSub_);

	// ===== 操作ヒント（上に配置）=====
	const TCHAR* hintSelect = TEXT("AD or ←→で選択");
	const TCHAR* hintEnter = TEXT("Enterで決定");

	int h1w = GetDrawStringWidthToHandle(hintSelect, (int)_tcslen(hintSelect), fontHint_);
	int h2w = GetDrawStringWidthToHandle(hintEnter, (int)_tcslen(hintEnter), fontHint_);

	// サブタイトル直下に置く
	const int hintY0 = frameY + 230;
	DrawStringToHandle(frameX + frameW / 2 - h1w / 2, hintY0 + 0, hintSelect, GetColor(210, 210, 210), fontHint_);
	DrawStringToHandle(frameX + frameW / 2 - h2w / 2, hintY0 + 30, hintEnter, GetColor(210, 210, 210), fontHint_);

	const int innerTop = frameY + 260;
	const int innerBottom = frameY + frameH - 170; // ←下にヒント2行増えるので少し上げる

	// ===== 3枚カードにするが、今の見た目（大きめカード）を維持 =====
	const int gap = max(35, frameW / 30);
	const int cardW = min(520, (frameW - gap * 2) / 3);
	const int cardH = min(460, innerBottom - innerTop);

	const int leftX0 = frameX + (frameW - (cardW * 3 + gap * 2)) / 2;
	const int cardX0 = leftX0;
	const int cardX1 = leftX0 + cardW + gap;
	const int cardX2 = leftX0 + (cardW + gap) * 2;
	const int cardY = innerTop + (innerBottom - innerTop - cardH) / 2;

	// == カード順番出現 ==
	const float cardDur = 0.22f;
	const float cardDelay = 0.10f;

	const float t0 = easeOutCubic(introElapsedSec_ / cardDur);
	const float t1 = easeOutCubic((introElapsedSec_ - cardDelay) / cardDur);
	const float t2 = easeOutCubic((introElapsedSec_ - cardDelay * 2.0f) / cardDur);

	auto makeCardAnim = [&](float t, int dir)
		{
			const int a = (int)(255.0f * t);
			const int ox = (int)((1.0f - t) * (float)(dir * 30)); // -1,0,+1
			const int oy = (int)((1.0f - t) * 30.0f);
			return std::tuple<int, int, int>(a, ox, oy);
		};

	auto [a0, ox0, oy0] = makeCardAnim(t0, -1);
	auto [a1, ox1, oy1] = makeCardAnim(t1, 0);
	auto [a2, ox2, oy2] = makeCardAnim(t2, 1);

	const int s1Fill = GetColor(110, 170, 70);
	const int s2Fill = GetColor(220, 40, 40);
	const int s3Fill = GetColor(120, 90, 220);
	const int cardBorder = GetColor(30, 80, 160);

	const int radius = 24;

	auto drawRoundCard = [&](int x, int y, int w, int h, int fillCol)
		{
			DrawRoundRect(x, y, x + w, y + h, radius, radius, fillCol, TRUE);
			DrawRoundRect(x, y, x + w, y + h, radius, radius, cardBorder, FALSE);
		};

	// ===== shadows =====
	auto drawShadow = [&](int x, int y, int w, int h, int alpha)
		{
			if (alpha <= 0) return;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(110.0f * (alpha / 255.0f)));
			DrawRoundRect(x + 10, y + 12, x + w + 10, y + h + 12, radius, radius, GetColor(0, 0, 0), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		};

	drawShadow(cardX0 + ox0, cardY + oy0, cardW, cardH, a0);
	drawShadow(cardX1 + ox1, cardY + oy1, cardW, cardH, a1);
	drawShadow(cardX2 + ox2, cardY + oy2, cardW, cardH, a2);

	// ===== bodies =====
	auto drawBody = [&](int x, int y, int w, int h, int col, int alpha)
		{
			if (alpha <= 0) return;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			drawRoundCard(x, y, w, h, col);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		};

	drawBody(cardX0 + ox0, cardY + oy0, cardW, cardH, s1Fill, a0);
	drawBody(cardX1 + ox1, cardY + oy1, cardW, cardH, s2Fill, a1);
	drawBody(cardX2 + ox2, cardY + oy2, cardW, cardH, s3Fill, a2);

	// ===== Selected highlight（既存の見た目）=====
	const bool blinkOn = ((blink_ / 18) % 2) == 0;
	const int hiCol = GetColor(255, 235, 150);

	if (introDone_ && blinkOn)
	{
		const int bx =
			(selected_ == 0) ? cardX0 :
			(selected_ == 1) ? cardX1 : cardX2;

		for (int i = 0; i < 8; ++i)
		{
			DrawRoundRect(bx - i, cardY - i, bx + cardW + i, cardY + cardH + i,
				radius + i, radius + i, hiCol, FALSE);
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
		DrawBox(bx + 18, cardY + 18, bx + cardW - 18, cardY + 54, GetColor(255, 255, 255), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	auto drawCenterLines = [&](int cx, int topY, const TCHAR* const* lines, int n, int lineGap,
		int fontHandle, int alpha)
		{
			if (alpha <= 0) return;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

			int y = topY;
			for (int i = 0; i < n; ++i)
			{
				const TCHAR* s = lines[i];
				int w = GetDrawStringWidthToHandle(s, (int)_tcslen(s), fontHandle);
				DrawStringToHandle(cx - w / 2, y, s, GetColor(255, 255, 255), fontHandle);
				y += lineGap;
			}

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		};

	// 要望：Stage3の説明文
	const TCHAR* stage1Lines[] = {
		TEXT("やさしい ちゅーとりある"),
		TEXT("すぐイライラしちゃう人に"),
		TEXT("おすすめ"),
		TEXT("残機数 5"),
	};
	const TCHAR* stage2Lines[] = {
		TEXT("むずかしい"),
		TEXT("たくさん死にたい人におすすめ"),
		TEXT("残機数 3"),
	};
	const TCHAR* stage3Lines[] = {
		TEXT("？？？"),
		TEXT("何があるかはお楽しみ"),
		TEXT("残機数 ？"),
	};

	drawCenterLines(cardX0 + ox0 + cardW / 2, (cardY + oy0) + 110, stage1Lines, 4, 50, fontCard_, a0);
	drawCenterLines(cardX1 + ox1 + cardW / 2, (cardY + oy1) + 135, stage2Lines, 3, 56, fontCard_, a1);
	drawCenterLines(cardX2 + ox2 + cardW / 2, (cardY + oy2) + 135, stage3Lines, 3, 56, fontCard_, a2);

	// ===== Bottom hints（要望の2行追加）=====
	const TCHAR* hintBack = TEXT("タイトルへ戻る  Push to [Space]");
	int w3 = GetDrawStringWidthToHandle(hintBack, (int)_tcslen(hintBack), fontHint_);
	DrawStringToHandle(frameX + frameW / 2 - w3 / 2, frameY + frameH - 60, hintBack, GetColor(188, 188, 188), fontHint_);


	DrawVignette(sw, sh);

	// 登場フェード
	if (introFadeA > 0 && !deciding_)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, introFadeA);
		DrawBox(0, 0, sw, sh, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// Fade after decide
	if (deciding_)
	{
		int a = (int)(255.0f * min(1.0f, max(0.0f, fade_)));
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, a);
		DrawBox(0, 0, sw, sh, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}
