#include "SelectStage.h"

#include <algorithm>

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

	fontTitle_ = CreateJPFont(TEXT("HGS創英角ﾎﾟｯﾌﾟ体"), 130, 4);
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

	introStartMs_ = GetNowCount();
	introT_ = 0.0f;
	introDone_ = false;
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
		// E、Hキーでステージ選択
		if (CheckHitKey(KEY_INPUT_E)) { selected_ = 0; Decide(1); return; }
		if (CheckHitKey(KEY_INPUT_H)) { selected_ = 1; Decide(2); return; }

		// 左右キーでも選べるよ
		if (CheckHitKey(KEY_INPUT_LEFT) || CheckHitKey(KEY_INPUT_A)) selected_ = 0;
		if (CheckHitKey(KEY_INPUT_RIGHT) || CheckHitKey(KEY_INPUT_F)) selected_ = 1;

		// 決定
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

	// 背景、黒い四角形
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
	DrawBox(0, 0, sw, sh, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SelectStage::DrawVignette(int sw, int sh) const
{
	// 簡易ビネット、ただの四角形４つ
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
	const int enterOffsetY = (int)((1.0f - e) * 50.0f); // 最初は下に50px → 0
	const int introFadeA = (int)((1.0f - e) * 255.0f); // 最初は黒255 → 0

	DrawTiledWall(sw, sh);

	// アウトフレーム
	const int marginX = max(60, sw / 16);
	const int marginY = max(50, sh / 14);
	const int frameX = marginX;
	const int frameY = marginY + enterOffsetY;  // 登場アニメぶんずらす
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

	DrawSparkles(sw, sh);

	// タイトル
	const TCHAR* title = TEXT("ステージ選択");
	int tw = GetDrawStringWidthToHandle(title, (int)_tcslen(title), fontTitle_);
	DrawStringToHandle(frameX + frameW / 2 - tw / 2, frameY + 30, title, GetColor(255, 180, 0), fontTitle_);

	// サブタイトル
	const TCHAR* sub = TEXT("挑戦したいステージのキーを押して下さい");
	int subw = GetDrawStringWidthToHandle(sub, (int)_tcslen(sub), fontSub_);
	DrawStringToHandle(frameX + frameW / 2 - subw / 2, frameY + 180, sub, GetColor(188, 188, 188), fontSub_);

	const int innerTop = frameY + 220;
	const int innerBottom = frameY + frameH - 140;

	const int gap = max(50, frameW / 12);
	const int cardW = min(640, (frameW - gap) / 2);
	const int cardH = min(460, innerBottom - innerTop);

	const int leftX = frameX + (frameW - (cardW * 2 + gap)) / 2;
	const int rightX = leftX + cardW + gap;
	const int cardY = innerTop + (innerBottom - innerTop - cardH) / 2;

	// == カード順番出現 ==
	const float cardDur = 0.22f; // 1枚が出る時間
	const float cardDelay = 0.10f; // 2枚目の遅延

	const float tL = easeOutCubic(introElapsedSec_ / cardDur);
	const float tR = easeOutCubic((introElapsedSec_ - cardDelay) / cardDur);

	// 出現度に応じて「スライド量」と「透明度」を作る
	auto makeCardAnim = [&](float t, bool fromLeft) {
		const int a = (int)(255.0f * t);                // 0→255
		const int ox = (int)((1.0f - t) * (fromLeft ? -30.0f : 30.0f)); // 左は左から/右は右から
		const int oy = (int)((1.0f - t) * 30.0f);       // 少し下から
		return std::tuple<int, int, int>(a, ox, oy);      // alpha, offsetX, offsetY
		};

	auto [aL, oxL, oyL] = makeCardAnim(tL, true);
	auto [aR, oxR, oyR] = makeCardAnim(tR, false);


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

	// ===== Card shadow（影もカードごとにフェード）=====
	if (aL > 0)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(110.0f * (aL / 255.0f)));
		DrawRoundRect(leftX + oxL + 10, cardY + oyL + 12,
			leftX + oxL + cardW + 10, cardY + oyL + cardH + 12,
			radius, radius, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	if (aR > 0)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(110.0f * (aR / 255.0f)));
		DrawRoundRect(rightX + oxR + 10, cardY + oyR + 12,
			rightX + oxR + cardW + 10, cardY + oyR + cardH + 12,
			radius, radius, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// ===== Card body（カード本体もフェード）=====
	if (aL > 0)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, aL);
		drawRoundCard(leftX + oxL, cardY + oyL, cardW, cardH, easyFill);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	if (aR > 0)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, aR);
		drawRoundCard(rightX + oxR, cardY + oyR, cardW, cardH, hardFill);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// ===== Selected highlight（カードが出切った後だけ光らせる）=====
	const bool blinkOn = ((blink_ / 18) % 2) == 0;
	const int hiCol = GetColor(255, 235, 150);

	if (introDone_ && blinkOn)
	{
		const int bx = (selected_ == 0) ? leftX : rightX;
		for (int i = 0; i < 8; ++i)
		{
			DrawRoundRect(bx - i, cardY - i, bx + cardW + i, cardY + cardH + i,
				radius + i, radius + i, hiCol, FALSE);
		}

		// Shine stripe
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

	drawCenterLines(leftX + oxL + cardW / 2, (cardY + oyL) + 90, easyLines, 5, 46, fontCard_, aL);
	drawCenterLines(rightX + oxR + cardW / 2, (cardY + oyR) + 110, hardLines, 4, 50, fontCard_, aR);

	// Bottom hint
	const TCHAR* bottom = TEXT("タイトルへ戻る  Push to [Space]");
	int bw = GetDrawStringWidthToHandle(bottom, (int)_tcslen(bottom), fontHint_);
	DrawStringToHandle(frameX + frameW / 2 - bw / 2, frameY + frameH - 80, bottom, GetColor(188, 188, 188), fontHint_);

	DrawVignette(sw, sh);

	// 登場フェード
	if (introFadeA > 0 && !deciding_) // deciding_中は決定フェードを優先したいので
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
