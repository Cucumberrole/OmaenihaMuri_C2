#include "TitleScene.h"
#include "SoundCache.h"

#include <DxLib.h>
#include <cmath>
#include <algorithm>

#include "../Library/Time.h"
#include "../Library/SceneManager.h"

#include "CsvReader.h"

// 画像パス
static const char* kBgPath = "data/image/kabe.png";
static const char* kLogoPath = "data/image/タイトルロゴ　背景透過済み.png";
static const char* kBlockPath = "data/image/NewBlock.png";
static const char* kPlayerPath = "data/image/OMAEwalk.png";

// タイトル用CSV
static const char* kTitleCsvPath = "data/title.csv";

// 乱数ユーティリティ
static float Rand01() { return GetRand(1000000) / 1000000.0f; }
static float RandRange(float a, float b) { return a + (b - a) * Rand01(); }

TitleScene::TitleScene()
{
	bgHandle = LoadGraph(kBgPath);
	logoHandle = LoadGraph(kLogoPath);
	blockHandle = LoadGraph(kBlockPath);
	playerSheetHandle = LoadGraph(kPlayerPath);

	if (logoHandle >= 0) GetGraphSize(logoHandle, &logoW, &logoH);

	if (blockHandle >= 0)
	{
		GetGraphSize(blockHandle, &blockW, &blockH);
		if (blockW <= 0) blockW = 64;
		if (blockH <= 0) blockH = 64;
	}

	if (playerSheetHandle >= 0)
	{
		GetGraphSize(playerSheetHandle, &playerSheetW, &playerSheetH);

		// OMAEwalk.png
		playerFrameH = (playerSheetH > 0) ? playerSheetH : 64;
		playerFrameW = playerFrameH;
		playerFrames = (playerFrameW > 0) ? (playerSheetW / playerFrameW) : 1;
		if (playerFrames < 1) playerFrames = 1;
	}

	// BGM
	sHandle = SoundCache::GetWithVolume("data/bgm/Title.mp3",100);
	PlaySoundMem(sHandle, DX_PLAYTYPE_LOOP);

	// CSVロード
	LoadTitleCsv(kTitleCsvPath);

	// ロゴバウンス初期化
	int sw, sh;
	GetDrawScreenSize(&sw, &sh);

	logoX = (sw - logoW) * 0.5f;
	logoTargetY = sh * 0.12f;
	logoY = -(float)logoH - 30.0f;
	logoVY = 0.0f;
	logoLanded = false;

	blinkTime = 0.0f;

	// パーティクル初期化
	for (int i = 0; i < kParticleCount; ++i) ResetParticle(i);
}

TitleScene::~TitleScene()
{
	if (bgHandle >= 0) DeleteGraph(bgHandle);
	if (logoHandle >= 0) DeleteGraph(logoHandle);
	if (blockHandle >= 0) DeleteGraph(blockHandle);
	if (playerSheetHandle >= 0) DeleteGraph(playerSheetHandle);

	if (sHandle >= 0) DeleteSoundMem(sHandle);
	
}

void TitleScene::LoadTitleCsv(const char* path)
{
	maps.clear();
	mapLoaded = false;
	spawned = false;

	CsvReader* csv = new CsvReader((char*)path);
	int lines = csv->GetLines();
	if (lines > 0)
	{
		maps.resize(lines);
		for (int y = 0; y < lines; ++y)
		{
			int cols = csv->GetColumns(y);
			maps[y].resize(cols);
			for (int x = 0; x < cols; ++x)
			{
				int v = csv->GetInt(y, x);
				maps[y][x] = v;

				if (!spawned && v == 2)
				{
					px = (float)(x * 64);
					py = (float)(y * 64);
					spawned = true;
				}
			}
		}
		mapLoaded = true;
	}
	delete csv;

	if (!spawned)
	{
		// CSVに2が無い場合の保険
		px = 64.0f;
		py = 64.0f;
		spawned = true;
	}
}

bool TitleScene::IsSolidCell(int tx, int ty) const
{
	if (!mapLoaded) return false;
	if (ty < 0 || ty >= (int)maps.size()) return false;
	if (tx < 0 || tx >= (int)maps[ty].size()) return false;

	return maps[ty][tx] == 1;
}

int TitleScene::HitCheckRight(int px_, int py_) const
{
	if (py_ < 0) return 0;

	int x = px_ / 64;
	int y = py_ / 64;

	if (IsSolidCell(x, y))
	{
		return (px_ % 64) + 1; // 右にめり込んだ分+1 押し戻し
	}
	return 0;
}

int TitleScene::HitCheckLeft(int px_, int py_) const
{
	if (py_ < 0) return 0;

	int x = px_ / 64;
	int y = py_ / 64;

	if (IsSolidCell(x, y))
	{
		return 64 - (px_ % 64); // 左にめり込んだ分 押し戻し
	}
	return 0;
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
		p.vx = RandRange(-20.0f, 20.0f);
		p.vy = RandRange(-170.0f, -100.0f);
		p.size = RandRange(5.0f, 10.0f);
		p.ttl = RandRange(1.2f, 2.2f);
	}
	else
	{
		p.vx = RandRange(-10.0f, 10.0f);
		p.vy = RandRange(-45.0f, -20.0f);
		p.size = RandRange(2.0f, 4.5f);
		p.ttl = RandRange(2.5f, 4.5f);
	}

	p.life = RandRange(0.0f, p.ttl);
}

void TitleScene::Update()
{
	// SHIFTで次へ
	if (CheckHitKey(KEY_INPUT_LSHIFT) || CheckHitKey(KEY_INPUT_RSHIFT))
	{
		SceneManager::ChangeScene("STAGE");
		return;
	}
	if (CheckHitKey(KEY_INPUT_ESCAPE))
	{
		SceneManager::Exit();
		return;
	}

	const float dt = Time::DeltaTime();

	// -----------------------------
	// ロゴ
	// -----------------------------
	const float gravity = 2600.0f;
	const float bounce = 0.55f;
	const float stopV = 60.0f;

	if (!logoLanded)
	{
		logoVY += gravity * dt;
		logoY += logoVY * dt;

		if (logoY >= logoTargetY)
		{
			logoY = logoTargetY;
			if (logoVY > stopV) logoVY = -logoVY * bounce;
			else { logoVY = 0.0f; logoLanded = true; }
		}
	}

	// -----------------------------
	// プレイヤー横移動
	// -----------------------------
	int moveX = 0;
	const int WALK_SPEED = 4;

	if (CheckHitKey(KEY_INPUT_D))
	{
		moveX = WALK_SPEED;
		flip = false;
	}
	else if (CheckHitKey(KEY_INPUT_A))
	{
		moveX = -WALK_SPEED;
		flip = true;
	}

	if (moveX != 0)
	{
		px += (float)moveX;

		int push = 0;

		if (moveX > 0)
		{
			int p1 = HitCheckRight((int)(px + 63), (int)(py + 5));
			int p2 = HitCheckRight((int)(px + 63), (int)(py + 58));
			push = max(p1, p2);
		}
		else
		{
			int p1 = HitCheckLeft((int)(px + 0), (int)(py + 5));
			int p2 = HitCheckLeft((int)(px + 0), (int)(py + 58));
			push = max(p1, p2);
		}

		if (push > 0)
		{
			if (moveX > 0) px -= (float)push;
			else           px += (float)push;
		}
	}

	// 歩行アニメーション
	if (playerFrames > 1)
	{
		if (moveX != 0)
		{
			animTime += dt;
			const float fps = 10.0f;
			animFrame = ((int)(animTime * fps)) % playerFrames;
		}
		else
		{
			animFrame = 0;
			animTime = 0.0f;
		}
	}

	// 画面外に出ないように制限
	int sw, sh;
	GetDrawScreenSize(&sw, &sh);
	if (px < 0) px = 0;
	if (px > sw - playerFrameW) px = (float)(sw - playerFrameW);

	// 点滅
	blinkTime += dt;

	// パーティクル更新
	for (int i = 0; i < kParticleCount; ++i)
	{
		Particle& p = particles[i];
		p.life += dt;

		float wobble = (p.kind == 1) ? 18.0f : 10.0f;
		float t = p.life * ((p.kind == 1) ? 7.0f : 5.0f);

		p.x += (p.vx + sinf(t) * wobble) * dt;
		p.y += p.vy * dt;

		if (p.life >= p.ttl || p.y < -60.0f || p.x < -80.0f || p.x >(float)sw + 80.0f)
		{
			ResetParticle(i);
		}
	}
}

void TitleScene::Draw()
{
	int sw, sh;
	GetDrawScreenSize(&sw, &sh);

	// -----------------------------
	// 背景
	// -----------------------------
	if (bgHandle >= 0) DrawExtendGraph(0, 0, sw, sh, bgHandle, FALSE);
	else DrawBox(0, 0, sw, sh, GetColor(80, 0, 0), TRUE);

	// -----------------------------
	// ブロック
	// -----------------------------
	if (mapLoaded && blockHandle >= 0)
	{
		for (int y = 0; y < (int)maps.size(); ++y)
		{
			for (int x = 0; x < (int)maps[y].size(); ++x)
			{
				if (maps[y][x] == 1)
				{
					int dx = x * 64;
					int dy = y * 64;
					if (dx < sw && dy < sh)
						DrawGraph(dx, dy, blockHandle, TRUE);
				}
			}
		}
	}

	// -----------------------------
	// パーティクル
	// -----------------------------
	for (int i = 0; i < kParticleCount; ++i)
	{
		const Particle& p = particles[i];

		float a;
		float half = p.ttl * 0.5f;
		if (p.life < half) a = p.life / half;
		else              a = (p.ttl - p.life) / half;

		a = std::clamp(a, 0.0f, 1.0f);

		if (p.kind == 1)
		{
			SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(120 + 120 * a));
			DrawCircle((int)p.x, (int)p.y, (int)(p.size * 1.8f), GetColor(255, 160, 40), TRUE);
			DrawCircle((int)p.x, (int)p.y, (int)(p.size * 1.0f), GetColor(255, 220, 120), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		else
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(40 + 60 * a));
			DrawCircle((int)p.x, (int)p.y, (int)p.size, GetColor(220, 220, 220), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}

	// -----------------------------
	// プレイヤー描画
	// -----------------------------
	if (playerSheetHandle >= 0)
	{
		int sx = animFrame * playerFrameW;
		int sy = 0;

		if (!flip)
		{
			DrawRectGraph((int)px, (int)py, sx, sy, playerFrameW, playerFrameH, playerSheetHandle, TRUE);
		}
		else
		{
			// 左向き
			DrawRectRotaGraph((int)px + playerFrameW / 2, (int)py + playerFrameH / 2,
				sx, sy, playerFrameW, playerFrameH,
				1.0, 0.0, playerSheetHandle, TRUE, TRUE);
		}
	}

	// -----------------------------
	// ロゴ
	// -----------------------------
	if (logoHandle >= 0)
	{
		float wobble = logoLanded ? sinf(blinkTime * 2.0f) * 2.5f : 0.0f;
		int x = (int)logoX;
		int y = (int)(logoY + wobble);

		DrawGraph(x + 6, y + 6, logoHandle, TRUE); // 影
		DrawGraph(x, y, logoHandle, TRUE);
	}

	// -----------------------------
	// push to shift
	// -----------------------------
	const char* text = "push to shift";
	SetFontSize(36);

	int th = GetFontSize();
	int tw = GetDrawStringWidth(text, -1);
	int tx = (sw - tw) / 2;
	int ty = sh - 95;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
	DrawBox(tx - 40, ty - 18, tx + tw + 40, ty + th + 18, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	float s = (sinf(blinkTime * 4.0f) + 1.0f) * 0.5f;
	int c = (int)(170 + 85 * s);

	DrawString(tx - 2, ty, text, GetColor(0, 0, 0));
	DrawString(tx + 2, ty, text, GetColor(0, 0, 0));
	DrawString(tx, ty - 2, text, GetColor(0, 0, 0));
	DrawString(tx, ty + 2, text, GetColor(0, 0, 0));
	DrawString(tx, ty, text, GetColor(c, c, c));
}
