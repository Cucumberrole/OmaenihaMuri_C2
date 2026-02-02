#include "GameOver.h"
#include "Screen.h"
#include "Player.h"
#include "PlayScene.h"
#include <ctime>

static float Clamp01(float x) { return (x < 0.f) ? 0.f : (x > 1.f ? 1.f : x); }

static float EaseOutCubic(float t)
{
	t = Clamp01(t);
	float u = 1.f - t;
	return 1.f - u * u * u;
}

static float EaseOutBack(float t)
{
	t = Clamp01(t);
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	return 1.0f + c3 * (t - 1.0f) * (t - 1.0f) * (t - 1.0f) + c1 * (t - 1.0f) * (t - 1.0f);
}

static int LerpInt(int a, int b, float t)
{
	t = Clamp01(t);
	return a + (int)((b - a) * t + 0.5f);
}

static float LerpFloat(float a, float b, float t)
{
	t = Clamp01(t);
	return a + (b - a) * t;
}

static int s_gameOverShownCount = 0;

static bool s_seeded = false;
static int PickRandomMsg(int m1, int m2, int m3, int m4, int m5, int m6, int m7, int m8, int m9)
{
	if (!s_seeded) { s_seeded = true; srand((unsigned)time(NULL)); }
	const int r = rand() % 9;
	const int arr[9] = { m1,m2,m3,m4,m5,m6,m7,m8,m9 };
	return arr[r];
}

GameOver::GameOver()
{
	//srand((unsigned)time(NULL));
	//Rand = rand() % 9 + 1;
	OwariImage = LoadGraph("data/Font/GAME OVER.png");//画像変えるときここ
	oneLineMsg = LoadGraph("data/font/ゲームオーバー画面文字/初のゲームオーバー.png");
	MsgImage1 = LoadGraph("data/font/ゲームオーバー画面文字/この死が君を強くする.png");//
	MsgImage2 = LoadGraph("data/font/ゲームオーバー画面文字/この死は意味があったのか.png");//
	MsgImage3 = LoadGraph("data/font/ゲームオーバー画面文字/ドンマイ！元気出して.png");//
	MsgImage4 = LoadGraph("data/font/ゲームオーバー画面文字/過去の屍を越えてゆけ.png");//
	MsgImage5 = LoadGraph("data/font/ゲームオーバー画面文字/完.png");//
	MsgImage6 = LoadGraph("data/font/ゲームオーバー画面文字/泣かないで….png");
	MsgImage7 = LoadGraph("data/font/ゲームオーバー画面文字/君はキャラの気持ちを考えたことはあるかい？.png");//
	MsgImage8 = LoadGraph("data/font/ゲームオーバー画面文字/止まるんじゃねぇぞ・・・.png");//
	MsgImage9 = LoadGraph("data/font/ゲームオーバー画面文字/疲れてるんじゃない？.png");//

	if (s_gameOverShownCount == 0)
	{
		// 初回固定
		Msg = oneLineMsg;
	}
	else
	{
		// ランダム
		Msg = PickRandomMsg(MsgImage1, MsgImage2, MsgImage3, MsgImage4, MsgImage5, MsgImage6, MsgImage7, MsgImage8, MsgImage9);
	}
	s_gameOverShownCount++;

	GameOverBGM = LoadSoundMem("data/BGM/Gameover.mp3");
	PlaySoundMem(GameOverBGM, DX_PLAYTYPE_LOOP);
	ChangeVolumeSoundMem(70, GameOverBGM);

	startMs_ = GetNowCount();
	t_ = 0.0f;
	fadeA_ = 0;
	titlePop_ = 0.85f;
	msgPop_ = 0.90f;
	shake_ = 0.0f;
	btnPulse_ = 0.0f;
}

GameOver::~GameOver()
{
	StopSoundMem(GameOverBGM);
}

void GameOver::Update()
{
	PlayScene* playscene = FindGameObject<PlayScene>();
	retry = playscene->GetRetry(retry);

	// アニメーション表示
	const int now = GetNowCount();
	t_ = (now - startMs_) / 1000.0f;

	// フェード
	{
		float a = EaseOutCubic(t_ / 0.35f);
		fadeA_ = LerpInt(0, 255, a);
	}

	{
		float p = (t_ - 0.05f) / 0.40f;
		titlePop_ = LerpFloat(0.85f, 1.0f, EaseOutBack(p));
	}

	{
		float p = (t_ - 0.18f) / 0.37f;
		msgPop_ = LerpFloat(0.90f, 1.0f, EaseOutBack(p));
	}

	{
		float s = 1.0f - Clamp01(t_ / 0.6f);
		shake_ = 6.0f * s;
	}

	{
		float u = t_ - 0.8f;
		if (u < 0.f) u = 0.f;
		btnPulse_ = 0.5f + 0.5f * sin(u * 3.6f); // 0..1
	}

	if (CheckHitKey(KEY_INPUT_T)) {
		s_gameOverShownCount = 0;
		SceneManager::ChangeScene("TITLE");
	}

	if (CheckHitKey(KEY_INPUT_R)) {
		SceneManager::ChangeScene("PLAY");
	}

	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}
}

void GameOver::Draw()
{
	int sw, sh;
	GetDrawScreenSize(&sw, &sh);
	const int oldFontSize = GetFontSize();

	// Backround
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, fadeA_);
	DrawBox(0, 0, sw, sh, GetColor(0, 0, 0), TRUE);

	{
		const int stripeH = 40;
		for (int i = 0; i < sh; i += stripeH)
		{
			int a = 18 + (int)(10 * sin((t_ * 1.5f) + i * 0.06f));
			if (a < 0) a = 0;
			if (a > 40) a = 40;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (fadeA_ * a) / 255);
			DrawBox(0, i, sw, i + stripeH / 2, GetColor(255, 120, 30), TRUE);
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, fadeA_);
	}

	// サイズ
	int titleW = 0, titleH = 0;
	GetGraphSize(OwariImage, &titleW, &titleH);

	int msgW = 0, msgH = 0;
	GetGraphSize(Msg, &msgW, &msgH);

	const int marginTop = sh / 12;
	const int gap1 = sh / 30;
	const int gap2 = sh / 18;

	const int sx = (int)lround((t_ * 22.0f) * shake_);
	const int sy = (int)lround(cos(t_ * 19.0f) * shake_);

	// ゲームオーバー画像
	{
		const int cx = sw / 2 + sx;
		const int cy = marginTop + titleH / 2 + sy;

		const int w = (int)lround(titleW * titlePop_);
		const int h = (int)lround(titleH * titlePop_);
		const int x = cx - w / 2;
		const int y = cy - h / 2;

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (fadeA_ * 90) / 255);
		DrawBox(x - 18, y - 10, x + w + 18, y + h + 10, GetColor(255, 120, 30), TRUE);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, fadeA_);
		DrawExtendGraph(x, y, x + w, y + h, OwariImage, TRUE);
	}

	// メッセージ
	int msgY = marginTop + (int)(titleH * titlePop_) + gap1;
	{
		const int cx = sw / 2;
		const int cy = msgY + msgH / 2;

		const int w = (int)lround(msgW * msgPop_);
		const int h = (int)lround(msgH * msgPop_);
		const int x = cx - w / 2;
		const int y = cy - h / 2;

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (fadeA_ * 55) / 255);
		DrawBox(x - 20, y + h / 2 - 10, x + w + 20, y + h / 2 + 10, GetColor(255, 200, 120), TRUE);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, fadeA_);
		DrawExtendGraph(x, y, x + w, y + h, Msg, TRUE);

		msgY = y;
	}

	// ボタン類
	const int buttonW = (sw < 1200) ? (sw * 2 / 3) : 520;
	const int buttonH = 90;
	const int buttonGap = 30;

	int buttonX = (sw - buttonW) / 2;
	int buttonY1 = msgY + (int)(msgH * msgPop_) + gap2;
	int buttonY2 = buttonY1 + buttonH + buttonGap;

	int bottom = buttonY2 + buttonH + marginTop / 2;
	if (bottom > sh)
	{
		int pullUp = bottom - sh;
		buttonY1 -= pullUp;
		buttonY2 -= pullUp;
	}

	const float grow = 1.0f + 0.02f * btnPulse_;
	const int bw = (int)lround(buttonW * grow);
	const int bh = (int)lround(buttonH * grow);
	const int bx = buttonX + (buttonW - bw) / 2;

	const int aPulse = (fadeA_ * (180 + (int)(50 * btnPulse_))) / 255;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, aPulse);

	DrawBox(bx - 10, buttonY1 - 10, bx + bw + 10, buttonY1 + bh + 10, GetColor(255, 120, 30), TRUE);
	DrawBox(bx - 10, buttonY2 - 10, bx + bw + 10, buttonY2 + bh + 10, GetColor(255, 120, 30), TRUE);

	DrawBox(bx, buttonY1, bx + bw, buttonY1 + bh, GetColor(255, 163, 30), TRUE);
	DrawBox(bx, buttonY2, bx + bw, buttonY2 + bh, GetColor(255, 163, 30), TRUE);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, fadeA_);

	SetFontSize(32);
	const int fontH = GetFontSize();

	auto DrawCenteredTextInBox = [&](int x1, int y1, int x2, int y2, const char* text)
		{
			int tw = GetDrawStringWidth(text, -1);
			int tx = x1 + ((x2 - x1) - tw) / 2;
			int ty = y1 + ((y2 - y1) - fontH) / 2;
			DrawString(tx, ty, text, GetColor(255, 255, 255));
		};

	DrawCenteredTextInBox(bx, buttonY1, bx + bw, buttonY1 + bh, "Rキーでリトライ");
	DrawCenteredTextInBox(bx, buttonY2, bx + bw, buttonY2 + bh, "Tキーでタイトルへ戻る");

	SetFontSize(20);
	DrawString(20, sh - 40, "ESC : Exit", GetColor(180, 180, 180));

	SetFontSize(oldFontSize);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
