#include "TitleScene.h"

#include <DxLib.h>
#include <cmath>

#include "PlayScene.h"
#include "GameResult.h"
#include "Screen.h"
#include "../Library/Time.h"
#include "../Library/SceneManager.h"

// 背景
static const char* kBgPath = "data/image/kabe.png";

// タイトルロゴ
static const char* kLogoPath = "data/image/タイトルロゴ　背景透過済み.png";

static float Rand01()
{
	return GetRand(1000000) / 1000000.0f;
}

static float RandRange(float a, float b)
{
	return a + (b - a) * Rand01();
}


static void DrawTiledBackground(int bgHandle, int bgW, int bgH)
{
	int sw, sh;
	GetDrawScreenSize(&sw, &sh);

	if (bgHandle < 0 || bgW <= 0 || bgH <= 0)
	{
		DrawBox(0, 0, sw, sh, GetColor(80, 0, 0), TRUE);
		return;
	}

	for (int y = 0; y < sh; y += bgH)
	{
		for (int x = 0; x < sw; x += bgW)
		{
			DrawGraph(x, y, bgHandle, FALSE);
		}
	}
}

TitleScene::TitleScene()
{
	TitleImage = LoadGraph(kBgPath);
	LogoImage = LoadGraph(kLogoPath);

	if (TitleImage >= 0) GetGraphSize(TitleImage, &bgW, &bgH);
	if (LogoImage >= 0)  GetGraphSize(LogoImage, &logoW, &logoH);

	// BGM
	sHandle = LoadSoundMem("data/bgm/002.ogg");
	if (sHandle >= 0)
	{
		PlaySoundMem(sHandle, DX_PLAYTYPE_LOOP);
	}

	// 画面サイズ
	int sw, sh;
	GetDrawScreenSize(&sw, &sh);

	// ロゴ配置：中央、Yは上から落下して止まる
	logoX = (sw - logoW) * 0.5f;
	logoTargetY = sh * 0.12f;                 // 好みで調整（上寄り）
	logoY = -static_cast<float>(logoH) - 30;  // 画面外から開始
	logoVY = 0.0f;
	logoLanded = false;

	blinkTime = 0.0f;

	for (int i = 0; i < kParticleCount; ++i)
	{
		ResetParticle(i);
	}

}

TitleScene::~TitleScene()
{
	if (TitleImage >= 0) DeleteGraph(TitleImage);
	if (LogoImage >= 0) DeleteGraph(LogoImage);

	if (sHandle >= 0) DeleteSoundMem(sHandle);
}

void TitleScene::Update()
{
	// SHIFTで次へ
	if (CheckHitKey(KEY_INPUT_LSHIFT) || CheckHitKey(KEY_INPUT_RSHIFT))
	{
		SceneManager::ChangeScene("STAGE");
		g_ClearTimeSeconds = 0.0f;
		g_RetryCount = 0;
		return;
	}

	if (CheckHitKey(KEY_INPUT_ESCAPE))
	{
		SceneManager::Exit();
		return;
	}

	// ロゴ
	const float dt = Time::DeltaTime();

	const float gravity = 2600.0f; // 落下の速さ
	const float bounce = 0.55f;    // 反発
	const float stopV = 60.0f;     // これ以下なら停止扱い

	if (!logoLanded)
	{
		logoVY += gravity * dt;
		logoY += logoVY * dt;

		if (logoY >= logoTargetY)
		{
			logoY = logoTargetY;

			if (logoVY > stopV)
			{
				logoVY = -logoVY * bounce;
			}
			else
			{
				logoVY = 0.0f;
				logoLanded = true;
			}
		}
	}

	// 点滅用
	blinkTime += dt;

	// --- パーティクル更新 ---
	for (int i = 0; i < kParticleCount; ++i)
	{
		Particle& p = particles[i];
		p.life += dt;

		float wobble = (p.kind == 1) ? 18.0f : 10.0f;
		float t = p.life * ((p.kind == 1) ? 7.0f : 5.0f);

		p.x += (p.vx + sinf(t) * wobble) * dt;
		p.y += p.vy * dt;

		if (p.life >= p.ttl || p.y < -60.0f || p.x < -80.0f || p.x > 2000.0f)
		{
			ResetParticle(i);
		}
	}

}

void TitleScene::Draw()
{
	int sw, sh;
	GetDrawScreenSize(&sw, &sh);

	DrawTiledBackground(TitleImage, bgW, bgH);

	if (LogoImage >= 0)
	{
		DrawGraph(static_cast<int>(logoX), static_cast<int>(logoY), LogoImage, TRUE);
	}

	// 画面下の文字
	const char* text = "push to shift";

	// 点滅
	const float s = (sinf(blinkTime * 4.0f) + 1.0f) * 0.5f; // 0..1
	const int c = static_cast<int>(160 + 95 * s);          // 160..255

	SetFontSize(36);
	int fontH = GetFontSize();
	int tw = GetDrawStringWidth(text, -1);

	int tx = (sw - tw) / 2;
	int ty = sh - 90;

	// 影
	DrawString(tx + 2, ty + 2, text, GetColor(0, 0, 0));
	// 本体
	DrawString(tx, ty, text, GetColor(c, c, c));

	// --- パーティクル描画（背景の上、ロゴの後ろ） ---
	for (int i = 0; i < kParticleCount; ++i)
	{
		const Particle& p = particles[i];

		// 0..1 フェード（出現→消える）
		float a = 1.0f;
		float half = p.ttl * 0.5f;
		if (p.life < half) a = p.life / half;
		else              a = (p.ttl - p.life) / half;

		int alpha = (int)(255 * a);
		if (alpha < 0) alpha = 0;
		if (alpha > 255) alpha = 255;

		if (p.kind == 1)
		{
			// 火の粉
			SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(120 + 120 * a));
			DrawCircle((int)p.x, (int)p.y, (int)p.size, GetColor(255, 200, 80), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		else
		{
			// ほこり
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(40 + 60 * a));
			DrawCircle((int)p.x, (int)p.y, (int)p.size, GetColor(220, 220, 220), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}

}

void TitleScene::ResetParticle(int i)
{
	int sw, sh;
	GetDrawScreenSize(&sw, &sh);

	Particle& p = particles[i];

	p.x = RandRange(0.0f, (float)sw);
	p.y = RandRange((float)sh * 0.35f, (float)sh + 40.0f);

	p.kind = (GetRand(99) < 30) ? 1 : 0;

	if (p.kind == 1)
	{
		// 火の粉
		p.vx = RandRange(-20.0f, 20.0f);
		p.vy = RandRange(-140.0f, -70.0f);
		p.size = RandRange(5.5f, 10.0f);
		p.ttl = RandRange(1.2f, 2.2f);
	}
	else
	{
		// ほこり
		p.vx = RandRange(-10.0f, 10.0f);
		p.vy = RandRange(-45.0f, -20.0f);
		p.size = RandRange(8.0f, 12.5f);
		p.ttl = RandRange(2.5f, 4.5f);
	}

	p.life = RandRange(0.0f, p.ttl); // バラけさせる
}
