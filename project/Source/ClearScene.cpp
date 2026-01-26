#include "ClearScene.h"
#include "GameResult.h"
#include "../Library/SceneManager.h"
#include <DxLib.h>
#include <cmath>
#include <cstdio>

// Image (64x64) shown on the right side.
// Put the file under the executable working directory.
static const char* kCharPath = "data/image/omae.png";

// Title images
static const char* kTitleClearPath = "data/Font/GAMECLEAR.png";
static const char* kTitleThanksPath = "data/Font/thankyou.png";

static int s_titleRefCount = 0;
static int s_imgTitleClear = -1;
static int s_imgTitleThanks = -1;


ClearScene::ClearScene()
{
	// 結果を確定
	GR_FixOnGoalOnce();

	// GameResult から「確定済みの値」をコピー
	clearTime = (float)g_GameResult.elapsedMs / 1000.0f;   // 秒
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

	frame = 0;
	InitConfetti();
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
}

void ClearScene::Update()
{
	++frame;

	// Back to title.
	if (CheckHitKey(KEY_INPUT_T) || CheckHitKey(KEY_INPUT_RETURN) || CheckHitKey(KEY_INPUT_SPACE))
	{
		SceneManager::ChangeScene("TITLE");
		return;
	}

	// Exit application.
	if (CheckHitKey(KEY_INPUT_ESCAPE))
	{
		SceneManager::Exit();
		return;
	}

	UpdateConfetti();
}

void ClearScene::Draw()
{
	DrawBackground();
	DrawConfetti();

	const int W = Screen::WIDTH;
	const int H = Screen::HEIGHT;

	// Title images
	int titleBottomY = 0;

	// Draw GAME CLEAR image
	if (s_imgTitleClear >= 0)
	{
		int iw = 0, ih = 0;
		GetGraphSize(s_imgTitleClear, &iw, &ih);

		const int targetW = (int)(W * 0.62f);
		const int targetH = (iw > 0) ? (int)((float)ih * ((float)targetW / (float)iw)) : 0;

		const int x = (W - targetW) / 2;
		const int y = 28;
		DrawExtendGraph(x, y, x + targetW, y + targetH, s_imgTitleClear, TRUE);
		titleBottomY = y + targetH;
	}
	else
	{
		SetFontSize(100);
		DrawOutlinedText(W / 2 - GetDrawStringWidth("GAME CLEAR", -1) / 2, 55, "GAME CLEAR",
			GetColor(255, 220, 80), GetColor(80, 20, 0));
		titleBottomY = 155;
	}

	// Draw Thank you image
	int thankTopY = titleBottomY + 6;
	if (s_imgTitleThanks >= 0)
	{
		int iw = 0, ih = 0;
		GetGraphSize(s_imgTitleThanks, &iw, &ih);

		const int targetW = (int)(W * 0.72f);
		const int targetH = (iw > 0) ? (int)((float)ih * ((float)targetW / (float)iw)) : 0;

		const int x = (W - targetW) / 2;
		const int y = thankTopY;
		DrawExtendGraph(x, y, x + targetW, y + targetH, s_imgTitleThanks, TRUE);
		titleBottomY = y + targetH;
	}
	else
	{
		SetFontSize(46);
		const char* sub = "Thank you for Playing!";
		DrawOutlinedText(W / 2 - GetDrawStringWidth(sub, -1) / 2, thankTopY, sub,
			GetColor(255, 240, 200), GetColor(80, 20, 0));
		titleBottomY = thankTopY + 52;
	}

	// One-line message (under Thank you)
	int msgBottomY = titleBottomY;
	if (!oneLineMsg.empty())
	{
		SetFontSize(34);
		const int msgY = titleBottomY + 10;
		const int msgX = W / 2 - GetDrawStringWidth(oneLineMsg.c_str(), -1) / 2;
		DrawOutlinedText(msgX, msgY, oneLineMsg.c_str(),
			GetColor(255, 240, 220), GetColor(80, 20, 0));
		msgBottomY = msgY + 40;
	}

	// Panels + character are centered as one block.

	const int panelW = 720;
	const int panelH = 110;
	const int gapY = 28;
	const int charScale = 6; // 64x64 -> 384x384
	const int charW = 64 * charScale;
	const int charH = 64 * charScale;
	const int gapX = 60;

	const int totalPanelH = panelH * 3 + gapY * 2;
	int topY = (H - totalPanelH) / 2 + 90;
	if (topY < msgBottomY + 70) topY = msgBottomY + 70;

	const int totalW = panelW + gapX + charW;
	const int panelX = (W - totalW) / 2;
	const int charX = panelX + panelW + gapX;

	// Panel 1: RANK
	DrawPanel(panelX, topY + 0 * (panelH + gapY), panelW, panelH);
	SetFontSize(40);
	DrawOutlinedText(panelX + 30, topY + 0 * (panelH + gapY) + 28, "RANK",
		GetColor(255, 235, 140), GetColor(60, 10, 0));
	SetFontSize(54);
	DrawOutlinedText(panelX + panelW - 150, topY + 0 * (panelH + gapY) + 18,
		rankText.c_str(), rankColor, GetColor(60, 10, 0));

	// Panel 2: CLEAR TIME
	DrawPanel(panelX, topY + 1 * (panelH + gapY), panelW, panelH);
	SetFontSize(40);
	DrawOutlinedText(panelX + 30, topY + 1 * (panelH + gapY) + 28, "CLEAR TIME",
		GetColor(255, 235, 140), GetColor(60, 10, 0));
	char timeBuf[32] = {};
	FormatTime(timeBuf);
	SetFontSize(40);
	DrawOutlinedText(panelX + panelW - 270, topY + 1 * (panelH + gapY) + 30,
		timeBuf, GetColor(255, 255, 255), GetColor(60, 10, 0));

	// Panel 3: SCORE
	DrawPanel(panelX, topY + 2 * (panelH + gapY), panelW, panelH);
	SetFontSize(40);
	DrawOutlinedText(panelX + 30, topY + 2 * (panelH + gapY) + 28, "SCORE",
		GetColor(255, 235, 140), GetColor(60, 10, 0));
	char scoreBuf[64] = {};
	std::snprintf(scoreBuf, sizeof(scoreBuf), "%d", finalScore);
	SetFontSize(40);
	DrawOutlinedText(panelX + panelW - 240, topY + 2 * (panelH + gapY) + 30,
		scoreBuf, GetColor(255, 255, 255), GetColor(60, 10, 0));


	// Footer hint
	SetFontSize(26);
	const char* hint = "Press T / Enter / Space to return to Title.  Esc: Exit";
	DrawOutlinedText(W / 2 - GetDrawStringWidth(hint, -1) / 2, H - 70, hint,
		GetColor(255, 240, 200), GetColor(80, 20, 0));

	// Character (right side)
	const float bob = std::sin(frame * 0.07f) * 10.0f;
	const int charY = topY + (totalPanelH - charH) / 2 + (int)bob;

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
}

void ClearScene::CalcScoreAndRank()
{
	// 重要：スコア/ランクは GameResult 側で確定済み。
	// ここではUI表現（文字/色/メッセージ）だけ作る。

	rankText.clear();
	rankText.push_back(rankChar);     // 既存UIに合わせて1文字表示のまま
	// もし "S RANK" 表示にしたいなら：
	// rankText = std::string(1, rankChar) + " RANK";

	if (rankChar == 'S') rankColor = GetColor(255, 240, 100);
	else if (rankChar == 'A') rankColor = GetColor(255, 210, 120);
	else if (rankChar == 'B') rankColor = GetColor(190, 230, 255);
	else if (rankChar == 'C') rankColor = GetColor(210, 210, 210);
	else rankColor = GetColor(255, 160, 160);

	// 一言メッセージの選択（既存テーブルをそのまま使用）
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
	case 'S': table = S_MSGS; count = (int)(sizeof(S_MSGS) / sizeof(S_MSGS[0])); break;
	case 'A': table = A_MSGS; count = (int)(sizeof(A_MSGS) / sizeof(A_MSGS[0])); break;
	case 'B': table = B_MSGS; count = (int)(sizeof(B_MSGS) / sizeof(B_MSGS[0])); break;
	case 'C': table = C_MSGS; count = (int)(sizeof(C_MSGS) / sizeof(C_MSGS[0])); break;
	default:  table = D_MSGS; count = (int)(sizeof(D_MSGS) / sizeof(D_MSGS[0])); break;
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
		c.x = (float)GetRand(Screen::WIDTH);
		c.y = (float)GetRand(Screen::HEIGHT);
		c.vy = 1.2f + (float)GetRand(140) / 60.0f;      // 1.2 .. 3.5
		c.vx = -0.7f + (float)GetRand(140) / 100.0f;    // -0.7 .. 0.7
		c.size = 6.0f + (float)GetRand(18);             // 6 .. 24
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
			c.y = -80.0f - (float)GetRand(200);
			c.x = (float)GetRand(Screen::WIDTH);
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
	const int cx = (int)(Screen::WIDTH * 0.52f);
	const int cy = (int)(Screen::HEIGHT * 0.18f);

	for (int i = 0; i < 20; ++i)
	{
		const float a = (float)i * (3.1415926f * 2.0f / 20.0f);
		const float r1 = (float)(Screen::WIDTH * 0.10f);
		const float r2 = (float)(Screen::WIDTH * 0.60f);

		const int x1 = (int)(cx + std::cos(a) * r1);
		const int y1 = (int)(cy + std::sin(a) * r1);
		const int x2 = (int)(cx + std::cos(a) * r2);
		const int y2 = (int)(cy + std::sin(a) * r2);

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
		const int x = (int)c.x;
		const int y = (int)c.y;
		const int s = (int)c.size;

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

void ClearScene::DrawOutlinedText(int x, int y, const char* text, unsigned int textColor, unsigned int outlineColor) const
{
	// 8-direction outline
	DrawString(x - 2, y, text, outlineColor);
	DrawString(x + 2, y, text, outlineColor);
	DrawString(x, y - 2, text, outlineColor);
	DrawString(x, y + 2, text, outlineColor);
	DrawString(x - 2, y - 2, text, outlineColor);
	DrawString(x + 2, y - 2, text, outlineColor);
	DrawString(x - 2, y + 2, text, outlineColor);
	DrawString(x + 2, y + 2, text, outlineColor);

	DrawString(x, y, text, textColor);
}

void ClearScene::FormatTime(char out[32]) const
{
	const float t = (clearTime < 0.0f) ? 0.0f : clearTime;
	const int totalMs = (int)std::round(t * 1000.0f);
	const int totalSec = totalMs / 1000;
	const int min = totalSec / 60;
	const int sec = totalSec % 60;
	const int ms = totalMs % 1000;

	std::snprintf(out, 32, "%02d:%02d.%03d", min, sec, ms);
}
