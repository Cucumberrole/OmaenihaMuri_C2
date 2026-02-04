#include "Sun.h"
#include <DxLib.h>
#include "Player.h"
#include "SoundCache.h"

Sun::Sun(float sx, float sy)
{
	hImage = LoadGraph("data/image/taiyouu.png");
	x = sx;
	y = sy;

	vx = 0.0f;
	vy = 0.0f;

	// 表示上のサイズ
	size = 64;

	moveTimer = 0.0f;
	changeInterval = 60.0f;

	int w, h;
	GetGraphSize(hImage, &w, &h);
	SunSE = SoundCache::Get("data/BGM/death_scream.mp3");
}

Sun::~Sun()
{
	DeleteGraph(hImage);
	StopSoundMem(SunSE);

}

void Sun::Update()
{
	moveTimer++;

	// 一定時間ごとに方向変更
	if (moveTimer >= changeInterval)
	{
		moveTimer = 0.0f;

		// -1.0 ～ 1.0 のランダム方向
		vx = (GetRand(200) - 100) / 100.0f;
		vy = (GetRand(200) - 100) / 100.0f;

		// スピード調整
		float speed = 1.5f;
		vx *= speed;
		vy *= speed;
	}

	// 移動
	x += vx;
	y += vy;

	// 画面外に出ないように反射
	if (x < 512 || x > 1950 - size) vx *= -1;
	if (y < 450 || y > 650 - size) vy *= -1;

	if (GetRand(100) < 3)
	{
		vx *= 1.5f;
		vy *= 1.5f;
	}

	Player* player = FindGameObject<Player>();
	if (player)
	{
		// プレイヤーの情報
		float px = player->GetX();
		float py = player->GetY();
		float pw = 64.0f;
		float ph = 64.0f;

		// プレイヤーの円
		float playerCx = px + pw / 2.0f;
		float playerCy = py + ph / 2.0f;
		float playerR = pw * 0.4f;

		// ボールの円（size は表示上の直径 64 を想定）
		float ballCx = x + size / 2.0f;
		float ballCy = y + size / 2.0f;
		float ballR = size * 0.5f;

		// 中心距離の2乗と半径の和の2乗で判定
		float dx = ballCx - playerCx;
		float dy = ballCy - playerCy;
		float dist2 = dx * dx + dy * dy;
		float rSum = ballR + playerR;

		if (dist2 <= rSum * rSum)
		{
			PlaySoundMem(SunSE, DX_PLAYTYPE_BACK);
			player->ForceDie();
			player->SetDead();
		}
	}
}

void Sun::Draw()
{
	float cx = x + size / 2.0f;
	float cy = y + size / 2.0f;

	DrawRotaGraph(
		static_cast<int>(cx),
		static_cast<int>(cy),
		1.7,
		0,
		hImage,
		TRUE
	);
}