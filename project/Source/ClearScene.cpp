#include "ClearScene.h"
#include "GameResult.h"
#include "../Library/SceneManager.h"
#include <DxLib.h>
#include <cmath>
#include <cstdio>

static const char* kCharPath = "data/image/omae.png";

static float Clamp01(float x) { return (x < 0.0f) ? 0.0f : (x > 1.0f ? 1.0f : x); }

static float EaseOutCubic(float t)
{
	t = Clamp01(t);
	float u = 1.0f - t;
	return 1.0f - u * u * u;
}

static float EaseOutBack(float t) // ちょい跳ねる
{
	t = Clamp01(t);
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	return 1.0f + c3 * (t - 1.0f) * (t - 1.0f) * (t - 1.0f) + c1 * (t - 1.0f) * (t - 1.0f);
}

static int LerpInt(int a, int b, float t)
{
	t = Clamp01(t);
	return (int)std::lround((double)a + ((double)b - (double)a) * (double)t);
}

static float LerpFloat(float a, float b, float t)
{
	t = Clamp01(t);
	return a + (b - a) * t;
}

static void FormatTimeSec(float sec, char out[32])
{
	if (sec < 0.0f) sec = 0.0f;
	const int totalMs = (int)std::lround(sec * 1000.0f);
	const int totalSec = totalMs / 1000;
	const int min = totalSec / 60;
	const int s = totalSec % 60;
	std::snprintf(out, 32, "%02d:%02d", min, s);
}

static const char* kTitleClearPath = "data/Font/GAMECLEAR.png";
static const char* kTitleThanksPath = "data/Font/thankyou.png";

static int s_titleRefCount = 0;
static int s_imgTitleClear = -1;
static int s_imgTitleThanks = -1;

static int CreateJPFont(const TCHAR* name, int size, int thick)
{
	int h = CreateFontToHandle(name, size, thick, DX_FONTTYPE_ANTIALIASING_8X8);
	return h;
}

static void DrawGraphKeepAspectCentered(
	int centerX, int topY, int maxW, int maxH, int graphHandle,
	int* outDstW = nullptr, int* outDstH = nullptr)
{
	int srcW = 0, srcH = 0;
	GetGraphSize(graphHandle, &srcW, &srcH);
	if (srcW <= 0 || srcH <= 0) return;

	const float sx = (float)maxW / (float)srcW;
	const float sy = (float)maxH / (float)srcH;
	const float s = (sx < sy) ? sx : sy;

	const int dstW = (int)(srcW * s + 0.5f);
	const int dstH = (int)(srcH * s + 0.5f);

	if (outDstW) *outDstW = dstW;
	if (outDstH) *outDstH = dstH;

	const int left = centerX - dstW / 2;
	DrawExtendGraph(left, topY, left + dstW, topY + dstH, graphHandle, TRUE);
}

ClearScene::ClearScene()
{
	// GameResult から「確定済みの値」をコピー
	clearTime = g_GameResult.elapsedMs / 1000.0f;   // 秒
	retryCount = g_GameResult.deathCount;            // 1回死ぬごとに
	finalScore = g_GameResult.score;
	rankChar = g_GameResult.rank;

	// 表示用テキスト・色・一言メッセージだけ決める
	CalcScoreAndRank();

	// Load title images (shared).
	if (++s_titleRefCount == 1)
	{
		s_imgTitleClear = LoadGraph(kTitleClearPath);
		s_imgTitleThanks = LoadGraph(kTitleThanksPath);
	}

	// Load character image (optional).
	imgChar = LoadGraph(kCharPath);

	// --- Fonts (SelectStage風フォールバック) ---
	fontTitle_ = CreateJPFont(TEXT("HGS創英角ﾎﾟｯﾌﾟ体"), 100, 4);
	if (fontTitle_ < 0) fontTitle_ = CreateJPFont(TEXT("Meiryo UI"), 84, 4);
	if (fontTitle_ < 0) fontTitle_ = CreateJPFont(TEXT("MS ゴシック"), 84, 3);
	if (fontTitle_ < 0) fontTitle_ = CreateJPFont(TEXT("Arial Black"), 84, 4);

	fontThanks_ = CreateJPFont(TEXT("HGS創英角ﾎﾟｯﾌﾟ体"), 46, 2);
	if (fontThanks_ < 0) fontThanks_ = CreateJPFont(TEXT("Meiryo UI"), 46, 2);
	if (fontThanks_ < 0) fontThanks_ = CreateJPFont(TEXT("MS ゴシック"), 46, 2);
	if (fontThanks_ < 0) fontThanks_ = CreateJPFont(TEXT("Arial"), 46, 2);

	fontMsg_ = CreateJPFont(TEXT("游明朝 Demibold"), 40, 2);
	if (fontMsg_ < 0) fontMsg_ = CreateJPFont(TEXT("Meiryo UI"), 34, 2);
	if (fontMsg_ < 0) fontMsg_ = CreateJPFont(TEXT("MS ゴシック"), 34, 2);
	if (fontMsg_ < 0) fontMsg_ = CreateJPFont(TEXT("Arial"), 34, 2);

	fontRankLabel_ = CreateJPFont(TEXT("HGS創英角ﾎﾟｯﾌﾟ体"), 82, 4);
	if (fontRankLabel_ < 0) fontRankLabel_ = CreateJPFont(TEXT("Meiryo UI"), 82, 4);
	if (fontRankLabel_ < 0) fontRankLabel_ = CreateJPFont(TEXT("MS ゴシック"), 82, 3);
	if (fontRankLabel_ < 0) fontRankLabel_ = CreateJPFont(TEXT("Arial Black"), 82, 4);

	fontRankValue_ = CreateJPFont(TEXT("HGS創英角ﾎﾟｯﾌﾟ体"), 118, 5);
	if (fontRankValue_ < 0) fontRankValue_ = CreateJPFont(TEXT("Meiryo UI"), 118, 5);
	if (fontRankValue_ < 0) fontRankValue_ = CreateJPFont(TEXT("MS ゴシック"), 118, 4);
	if (fontRankValue_ < 0) fontRankValue_ = CreateJPFont(TEXT("Arial Black"), 118, 5);

	fontPanel_ = CreateJPFont(TEXT("BIZ UDP明朝 Medium"), 40, 2);
	if (fontPanel_ < 0) fontPanel_ = CreateJPFont(TEXT("Meiryo UI"), 40, 2);
	if (fontPanel_ < 0) fontPanel_ = CreateJPFont(TEXT("MS ゴシック"), 40, 2);
	if (fontPanel_ < 0) fontPanel_ = CreateJPFont(TEXT("Arial"), 40, 2);

	fontHint_ = CreateJPFont(TEXT("メイリオ"), 26, 2);
	if (fontHint_ < 0) fontHint_ = CreateJPFont(TEXT("Meiryo UI"), 26, 2);
	if (fontHint_ < 0) fontHint_ = CreateJPFont(TEXT("MS ゴシック"), 26, 2);
	if (fontHint_ < 0) fontHint_ = CreateJPFont(TEXT("Arial"), 26, 2);

	frame = 0;
	InitConfetti();

	animStartMs_ = GetNowCount();
	lastMs_ = animStartMs_;
	animTime_ = 0.0f;
	animSkip_ = false;
	animDone_ = false;

	dispScore_ = 0;
	dispTimeSec_ = 0.0f;

	panelAlpha_ = 0;
	rankAlpha_ = 0;
	rankOffsetY_ = -20.0f;
	ChangeVolumeSoundMem(120, GoalBGM);
	GoalBGM = LoadSoundMem("data/BGM/bgm_result.mp3");
	PlaySoundMem(GoalBGM, DX_PLAYTYPE_LOOP);
}

ClearScene::~ClearScene()
{
	if (imgChar >= 0)
	{
		DeleteGraph(imgChar);
		imgChar = -1;
	}

	if (s_titleRefCount > 0 && --s_titleRefCount == 0)
	{
		if (s_imgTitleClear >= 0) { DeleteGraph(s_imgTitleClear); s_imgTitleClear = -1; }
		if (s_imgTitleThanks >= 0) { DeleteGraph(s_imgTitleThanks); s_imgTitleThanks = -1; }
	}
	if (fontTitle_ >= 0) { DeleteFontToHandle(fontTitle_); fontTitle_ = -1; }
	if (fontThanks_ >= 0) { DeleteFontToHandle(fontThanks_); fontThanks_ = -1; }
	if (fontMsg_ >= 0) { DeleteFontToHandle(fontMsg_); fontMsg_ = -1; }
	if (fontRankLabel_ >= 0) { DeleteFontToHandle(fontRankLabel_); fontRankLabel_ = -1; }
	if (fontRankValue_ >= 0) { DeleteFontToHandle(fontRankValue_); fontRankValue_ = -1; }
	if (fontPanel_ >= 0) { DeleteFontToHandle(fontPanel_); fontPanel_ = -1; }
	if (fontHint_ >= 0) { DeleteFontToHandle(fontHint_); fontHint_ = -1; }

	StopSoundMem(GoalBGM);
}

void ClearScene::Update()
{
	++frame;

	// Exit application.
	if (CheckHitKey(KEY_INPUT_ESCAPE))
	{
		SceneManager::Exit();
		return;
	}

	// アニメのスキップ or タイトルへ
	const bool pressReturn =
		CheckHitKey(KEY_INPUT_T) || CheckHitKey(KEY_INPUT_RETURN) || CheckHitKey(KEY_INPUT_SPACE);

	// dt
	const int now = GetNowCount();
	float dt = (now - lastMs_) / 1000.0f;
	if (dt < 0.0f) dt = 0.0f;
	if (dt > 0.1f) dt = 0.1f; // 突発の長フレーム対策
	lastMs_ = now;

	// Timeline (seconds)
	const float tFadeInS = 0.00f, tFadeInE = 0.25f;
	const float tTimeS = 0.20f, tTimeE = 1.10f;
	const float tScoreS = 0.55f, tScoreE = 1.90f;
	const float tRankS = 1.60f, tRankE = 2.20f;
	const float tEnd = 2.20f;

	if (pressReturn)
	{
		if (!animDone_) animSkip_ = true;   // アニメ中：スキップ
		else
		{
			SceneManager::ChangeScene("TITLE"); // アニメ後：タイトルへ
			return;
		}
	}

	if (!animDone_)
	{
		if (animSkip_)
		{
			animTime_ = tEnd;
		}
		else
		{
			animTime_ += dt;
			if (animTime_ >= tEnd) { animTime_ = tEnd; }
		}

		// Panel fade
		{
			float t = (animTime_ - tFadeInS) / (tFadeInE - tFadeInS);
			panelAlpha_ = (int)std::lround(255.0f * EaseOutCubic(t));
			if (panelAlpha_ < 0) panelAlpha_ = 0;
			if (panelAlpha_ > 255) panelAlpha_ = 255;
		}

		// Time count-up
		{
			float t = (animTime_ - tTimeS) / (tTimeE - tTimeS);
			float e = EaseOutCubic(t);
			dispTimeSec_ = LerpFloat(0.0f, clearTime, e);
		}

		// Score count-up
		{
			float t = (animTime_ - tScoreS) / (tScoreE - tScoreS);
			float e = EaseOutCubic(t);
			dispScore_ = LerpInt(0, finalScore, e);
		}

		// Rank appear (fade + bounce)
		{
			float t = (animTime_ - tRankS) / (tRankE - tRankS);
			float eA = EaseOutCubic(t);
			float eB = EaseOutBack(t);

			rankAlpha_ = (int)std::lround(255.0f * eA);
			if (rankAlpha_ < 0) rankAlpha_ = 0;
			if (rankAlpha_ > 255) rankAlpha_ = 255;

			// 上から落ちてくる＋少しバウンス
			rankOffsetY_ = LerpFloat(-24.0f, 0.0f, eB);
		}

		if (animTime_ >= tEnd)
		{
			animDone_ = true;
		}
	}

	UpdateConfetti();
}

void ClearScene::Draw()
{
	// Debug (temporary)
	//DrawFormatString(20, 20, GetColor(255, 255, 255), "elapsedMs=%d  score=%d  rank=%c", g_GameResult.elapsedMs, g_GameResult.score, g_GameResult.rank);

	DrawBackground();
	DrawConfetti();
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, panelAlpha_);

	const int W = Screen::WIDTH;
	const int H = Screen::HEIGHT;

	// Title images
	int titleBottomY = 0;

	// GAME CLEAR画像
	if (s_imgTitleClear >= 0)
	{
		int dstW = 0, dstH = 0;
		const int topY = 18;

		DrawGraphKeepAspectCentered(W / 2, topY,
			(int)(W * 0.70f), (int)(H * 0.22f),
			s_imgTitleClear, &dstW, &dstH);

		titleBottomY = topY + dstH;
	}
	else
	{
		DrawOutlinedTextToHandle(
			W / 2 - GetDrawStringWidthToHandle("GAME CLEAR", -1, fontTitle_) / 2,
			55, "GAME CLEAR",
			GetColor(255, 220, 80), GetColor(80, 20, 0), fontTitle_);

		titleBottomY = 155;
	}

	// Thank you 画像
	int thankTopY = titleBottomY + 6;
	if (s_imgTitleThanks >= 0)
	{
		int dstW = 0, dstH = 0;

		DrawGraphKeepAspectCentered(W / 2, thankTopY,
			(int)(W * 0.50f), (int)(H * 0.18f),
			s_imgTitleThanks, &dstW, &dstH);

		titleBottomY = thankTopY + dstH;
	}
	else
	{
		const char* sub = "Thank you for Playing!";
		DrawOutlinedTextToHandle(
			W / 2 - GetDrawStringWidthToHandle(sub, -1, fontThanks_) / 2,
			thankTopY, sub,
			GetColor(255, 240, 200), GetColor(80, 20, 0), fontThanks_);

		titleBottomY = thankTopY + 52;
	}

	// One-line message (under Thank you)
	int msgBottomY = titleBottomY;
	if (!oneLineMsg.empty())
	{
		const int msgY = titleBottomY + 10;
		const int msgX = W / 2 - GetDrawStringWidthToHandle(oneLineMsg.c_str(), -1, fontMsg_) / 2;

		DrawOutlinedTextToHandle(msgX, msgY, oneLineMsg.c_str(),
			GetColor(255, 240, 220), GetColor(80, 20, 0), fontMsg_);

		const int msgH = GetFontSizeToHandle(fontMsg_);
		msgBottomY = msgY + msgH + 6;
	}

	// Panels + character are centered as one block.

	const int panelW = 720;
	const int infoPanelW = 560;

	// Make RANK panel bigger (like the reference image)
	const int rankPanelH = 170;
	const int infoPanelH = 95;
	const int gapY = 26;

	const int charScale = 6; // 64x64 -> 384x384
	const int charW = 64 * charScale;
	const int charH = 64 * charScale;
	const int gapX = 60;

	const int totalPanelH = rankPanelH + infoPanelH * 2 + gapY * 2;
	int topY = (H - totalPanelH) / 2 + 90;
	if (topY < msgBottomY + 70) topY = msgBottomY + 70;

	const int totalW = panelW + gapX + charW;
	const int panelX = (W - totalW) / 2;
	const int charX = panelX + panelW + gapX;

	const int rankY = topY;
	const int timeY = rankY + rankPanelH + gapY;
	const int scoreY = timeY + infoPanelH + gapY;

	// Panel 1: RANK (big panel)
	DrawPanel(panelX, rankY, panelW, rankPanelH);

	// "RANK"
	const char* rankLabel = "RANK";
	DrawOutlinedTextToHandle(panelX + 30, rankY + 16, rankLabel,
		GetColor(255, 235, 140), GetColor(60, 10, 0), fontRankLabel_);

	// Rank value
	const int rankValueW = GetDrawStringWidthToHandle(rankText.c_str(), -1, fontRankValue_);
	const int rankValueX = panelX + (panelW - rankValueW) / 2;
	const int baseRankValueY = rankY + rankPanelH - 118 - 14;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (panelAlpha_ < rankAlpha_) ? panelAlpha_ : rankAlpha_);
	DrawOutlinedTextToHandle(rankValueX, baseRankValueY + (int)std::lround(rankOffsetY_), rankText.c_str(),
		rankColor, GetColor(60, 10, 0), fontRankValue_);

	// 以降のUIはパネルαに戻す
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, panelAlpha_);

	// --- 追加：表示用バッファを用意 ---
	char timeBuf[32] = {};
	FormatTimeSec(dispTimeSec_, timeBuf);

	char scoreBuf[64] = {};
	std::snprintf(scoreBuf, sizeof(scoreBuf), "%d", dispScore_);


	// CLEAR TIME
	DrawPanel(panelX, timeY, infoPanelW, infoPanelH);

	DrawOutlinedTextToHandle(panelX + 30, timeY + 24, "CLEAR TIME",
		GetColor(255, 235, 140), GetColor(60, 10, 0), fontPanel_);

	// 右端揃え
	{
		const int padR = 30;
		const int w = GetDrawStringWidthToHandle(timeBuf, -1, fontPanel_);
		const int x = panelX + infoPanelW - padR - w;
		DrawOutlinedTextToHandle(x, timeY + 26, timeBuf,
			GetColor(255, 255, 255), GetColor(60, 10, 0), fontPanel_);
	}

	// SCORE
	DrawPanel(panelX, scoreY, infoPanelW, infoPanelH);

	DrawOutlinedTextToHandle(panelX + 30, scoreY + 24, "SCORE",
		GetColor(255, 235, 140), GetColor(60, 10, 0), fontPanel_);

	// 右端揃え
	{
		const int padR = 30;
		const int w = GetDrawStringWidthToHandle(scoreBuf, -1, fontPanel_);
		const int x = panelX + infoPanelW - padR - w;
		DrawOutlinedTextToHandle(x, scoreY + 26, scoreBuf,
			GetColor(255, 255, 255), GetColor(60, 10, 0), fontPanel_);
	}


	// Footer hint
	const char* hint = "Press T / Enter / Space to return to Title.  Esc: Exit";
	DrawOutlinedTextToHandle(W / 2 - GetDrawStringWidthToHandle(hint, -1, fontHint_) / 2, H - 70, hint, GetColor(255, 240, 200), GetColor(80, 20, 0), fontHint_);


	// Character (right side)
	const float bob = std::sin(frame * 0.07f) * 10.0f;
	const int charY = topY + (totalPanelH - charH) / 2 + bob;

	if (imgChar >= 0)
	{
		DrawExtendGraph(charX, charY, charX + charW, charY + charH, imgChar, TRUE);
	}
	else
	{
		DrawBox(charX, charY, charX + charW, charY + charH, GetColor(255, 0, 0), FALSE);
		SetFontSize(24);
		DrawString(charX + 10, charY + 10, "IMG LOAD FAILED", GetColor(255, 255, 255));
	}

	// ---- DEBUG OVERLAY ----
	/*
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
		DrawBox(0, 0, 560, 90, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawFormatString(10, 10, GetColor(255, 255, 255),
			"elapsedMs=%d  clearTime=%.3f  score=%d  rank=%c",
			g_GameResult.elapsedMs, clearTime, g_GameResult.score, g_GameResult.rank);
	}
	*/

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ClearScene::CalcScoreAndRank()
{
	rankText.clear();
	rankText.push_back(rankChar);

	if (rankChar == 'S') rankColor = GetColor(255, 240, 100);
	else if (rankChar == 'A') rankColor = GetColor(255, 210, 120);
	else if (rankChar == 'B') rankColor = GetColor(190, 230, 255);
	else if (rankChar == 'C') rankColor = GetColor(210, 210, 210);
	else rankColor = GetColor(255, 160, 160);

	// 一言メッセージの選択
	static const char* const S_MSGS[] = {
		"\u7121\u7406\u3068\u304B\u8A00\u3063\u3066\u3059\u307F\u307E\u305B\u3093\u3067\u3057\u305F\u2026\u3042\u306A\u305F\u306F\u5929\u624D\u3067\u3059\u3002",
		"\u30A8\u30E9\u30FC\u3002\u4F5C\u8005\u306E\u60F3\u5B9A\u3092\u8D85\u3048\u307E\u3057\u305F\u3002",
		"\u6094\u3057\u3044\u3067\u3059\u304C\u5B8C\u6557\u3067\u3059",
		"\u3042\u306A\u305F\u304C\u30CA\u30F3\u30D0\u30FC1\u3060",
	};
	static const char* const A_MSGS[] = {
		"\u3084\u308A\u307E\u3059\u306D\u3047\uFF01\u3000S\u30E9\u30F3\u30AF\u307E\u3067\u3082\u3046\u5C11\u3057\uFF01",
		"\u8A87\u308C\u3002\u304A\u524D\u306F\u51C4\u3044\u3002",
		"\u3053\u3053\u307E\u3067\u6765\u305F\u306A\u3089\u3001\u884C\u3051\u308B",
		"\u307B\u307C\u5B8C\u58C1\u3002\u3042\u3068\u4E00\u6B69\u3067S\u30E9\u30F3\u30AF",
		"\u5224\u65AD\u306F\u6B63\u3057\u3044\u3002\u3042\u3068\u306F\u901F\u5EA6\u3060\u3051\u3002",
	};
	static const char* const B_MSGS[] = {
		"\u3046\u3093\u3001\u666E\u901A\u3002\u30B3\u30E1\u30F3\u30C8\u306B\u56F0\u308B\u3002",
		"\u60AA\u304F\u306A\u3044\u3002\u3060\u304C\u3001\u901F\u304F\u3082\u306A\u3044\u3002",
		"\u5B89\u5B9A\u30AF\u30EA\u30A2\u304A\u3081\u3067\u3068\u3046\uFF01\uFF01",
		"B\u30E9\u30F3\u30AF\u3063\u3066\u6B63\u76F4\u3001\u3057\u3083\u3070\u304F\u306A\u3044\u3067\u3059\u304B\uFF1F",
	};
	static const char* const C_MSGS[] = {
		"\u3082\u3046\u5C11\u3057\u771F\u9762\u76EE\u306B\u3084\u308D\u3063\u304B\u2026",
		"\u3060\u3044\u3076\u82E6\u6226\u3057\u3066\u305F\u306Dw",
		"\u6700\u77ED\u3078\u306E\u9053\u306F\u9577\u3044\u30FB\u30FB\u30FB",
		"\u307E\u305A\u306F\u30AF\u30EA\u30A2\u3059\u308B\u3053\u3068\u3092\u9811\u5F35\u308D\u3063\u304B",
	};
	static const char* const D_MSGS[] = {
		"\u51FA\u76F4\u3057\u3066\u304D\u3066\u306D\u2661",
		"\u541B\u304C\u8AE6\u3081\u306A\u304B\u3063\u305F\u3053\u3068\u306B\u611F\u52D5",
		"\u30AF\u30EA\u30A2\u3067\u304D\u3066\u3088\u304B\u3063\u305F\u306D",
		"D\u30E9\u30F3\u30AF\u306F\u53B3\u3057\u3044\u3063\u3066",
	};

	const char* const* table = nullptr;
	int count = 0;
	switch (rankChar)
	{
	case 'S': table = S_MSGS; count = (sizeof(S_MSGS) / sizeof(S_MSGS[0])); break;
	case 'A': table = A_MSGS; count = (sizeof(A_MSGS) / sizeof(A_MSGS[0])); break;
	case 'B': table = B_MSGS; count = (sizeof(B_MSGS) / sizeof(B_MSGS[0])); break;
	case 'C': table = C_MSGS; count = (sizeof(C_MSGS) / sizeof(C_MSGS[0])); break;
	default:  table = D_MSGS; count = (sizeof(D_MSGS) / sizeof(D_MSGS[0])); break;
	}

	if (table && count > 0) oneLineMsg = table[GetRand(count - 1)];
	else oneLineMsg.clear();
}

void ClearScene::InitConfetti()
{
	confetti.clear();
	confetti.reserve(220);

	for (int i = 0; i < 220; ++i)
	{
		Confetti c{};
		c.x = GetRand(Screen::WIDTH);
		c.y = GetRand(Screen::HEIGHT);
		c.vy = 1.2f + GetRand(140) / 60.0f;      // 1.2 .. 3.5
		c.vx = -0.7f + GetRand(140) / 100.0f;    // -0.7 .. 0.7
		c.size = 6.0f + GetRand(18);             // 6 .. 24
		c.kind = GetRand(2);

		const int pal = GetRand(4);
		if (pal == 0) c.color = GetColor(255, 230, 120);
		else if (pal == 1) c.color = GetColor(255, 180, 80);
		else if (pal == 2) c.color = GetColor(255, 255, 255);
		else c.color = GetColor(255, 120, 80);

		confetti.push_back(c);
	}
}

void ClearScene::UpdateConfetti()
{
	for (auto& c : confetti)
	{
		c.x += c.vx;
		c.y += c.vy;

		if (c.y > Screen::HEIGHT + 80.0f)
		{
			c.y = -80.0f - GetRand(200);
			c.x = GetRand(Screen::WIDTH);
		}
		if (c.x < -80.0f) c.x = Screen::WIDTH + 80.0f;
		if (c.x > Screen::WIDTH + 80.0f) c.x = -80.0f;
	}
}

void ClearScene::DrawBackground() const
{
	// Base red
	DrawBox(0, 0, Screen::WIDTH, Screen::HEIGHT, GetColor(210, 40, 30), TRUE);

	// Rays around the title area
	const int cx = (Screen::WIDTH * 0.52f);
	const int cy = (Screen::HEIGHT * 0.18f);

	for (int i = 0; i < 20; ++i)
	{
		const float a = i * (3.1415926f * 2.0f / 20.0f);
		const float r1 = (Screen::WIDTH * 0.10f);
		const float r2 = (Screen::WIDTH * 0.60f);

		const int x1 = (cx + std::cos(a) * r1);
		const int y1 = (cy + std::sin(a) * r1);
		const int x2 = (cx + std::cos(a) * r2);
		const int y2 = (cy + std::sin(a) * r2);

		DrawLine(x1, y1, x2, y2, GetColor(255, 120, 80));
	}

	// Sparkle dots (cheap pseudo-random)
	for (int i = 0; i < 160; ++i)
	{
		const int x = (i * 97 + frame * 3) % Screen::WIDTH;
		const int y = (i * 53 + frame * 2) % Screen::HEIGHT;
		DrawCircle(x, y, (i % 3) + 1, GetColor(255, 220, 160), TRUE);
	}
}

void ClearScene::DrawConfetti() const
{
	for (const auto& c : confetti)
	{
		const int x = c.x;
		const int y = c.y;
		const int s = c.size;

		if (c.kind == 0)
		{
			DrawBox(x, y, x + s, y + s, c.color, TRUE);
		}
		else if (c.kind == 1)
		{
			DrawTriangle(x, y, x + s, y + s / 2, x, y + s, c.color, TRUE);
		}
		else
		{
			DrawLine(x - s / 2, y, x + s / 2, y, c.color);
			DrawLine(x, y - s / 2, x, y + s / 2, c.color);
		}
	}
}

void ClearScene::DrawPanel(int x, int y, int w, int h) const
{
	const unsigned int edge = GetColor(120, 20, 10);
	const unsigned int outer = GetColor(250, 170, 60);
	const unsigned int inner = GetColor(200, 50, 35);

	DrawBox(x, y, x + w, y + h, edge, TRUE);
	DrawBox(x + 4, y + 4, x + w - 4, y + h - 4, outer, TRUE);
	DrawBox(x + 10, y + 10, x + w - 10, y + h - 10, inner, TRUE);

	// highlight bar
	DrawBox(x + 10, y + 10, x + w - 10, y + 16, GetColor(255, 230, 140), TRUE);
}

void ClearScene::DrawOutlinedTextToHandle(int x, int y, const char* text, unsigned int textColor, unsigned int outlineColor, int fontHandle) const
{
	DrawStringToHandle(x - 2, y, text, outlineColor, fontHandle);
	DrawStringToHandle(x + 2, y, text, outlineColor, fontHandle);
	DrawStringToHandle(x, y - 2, text, outlineColor, fontHandle);
	DrawStringToHandle(x, y + 2, text, outlineColor, fontHandle);
	DrawStringToHandle(x - 2, y - 2, text, outlineColor, fontHandle);
	DrawStringToHandle(x + 2, y - 2, text, outlineColor, fontHandle);
	DrawStringToHandle(x - 2, y + 2, text, outlineColor, fontHandle);
	DrawStringToHandle(x + 2, y + 2, text, outlineColor, fontHandle);

	DrawStringToHandle(x, y, text, textColor, fontHandle);
}

void ClearScene::FormatTime(char out[32]) const
{
	const float t = (clearTime < 0.0f) ? 0.0f : clearTime;
	const int totalMs = round(t * 1000.0f);
	const int totalSec = totalMs / 1000;
	const int min = totalSec / 60;
	const int sec = totalSec % 60;

	std::snprintf(out, 32, "%02d:%02d", min, sec);
}
