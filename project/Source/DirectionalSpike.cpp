#include "DirectionalSpike.h"
#include "Player.h"
#include "Collision.h"
#include <DxLib.h>

DirectionalSpike::DirectionalSpike(float sx, float sy, SpikeDir dir)
{
	x = sx;
	y = sy;
	this->dir = dir;

	hImage = LoadGraph("data/image/hari.png");

	if (hImage != -1)
	{
		int w, h;
		GetGraphSize(hImage, &w, &h);
		width = w;
		height = h;
	}
	SpikeSE = LoadSoundMem("data/BGM/spikeDeath.mp3");
}

DirectionalSpike::~DirectionalSpike()
{
	if (hImage != -1)
	{
		DeleteGraph(hImage);
	}
	StopSoundMem(SpikeSE);
}

void DirectionalSpike::Update()
{
	Player* player = FindGameObject<Player>();
	if (!player) return;

	// プレイヤーの当たり判定（円）
	float cx, cy, cr;
	player->GetHitCircle(cx, cy, cr);
	VECTOR center = VGet(cx, cy, 0.0f);

	float sx = x;
	float sy = y;

	VECTOR t1, t2, t3;

	switch (dir)
	{
	case SpikeDir::Up:
		// 上向き（三角の先端が上）
		t1 = VGet(sx, sy + height, 0.0f); // 左下
		t2 = VGet(sx + width, sy + height, 0.0f); // 右下
		t3 = VGet(sx + width / 2.0f, sy, 0.0f); // 上の先端
		break;

	case SpikeDir::Down:
		// 下向き（三角の先端が下）
		t1 = VGet(sx, sy, 0.0f); // 左上
		t2 = VGet(sx + width, sy, 0.0f); // 右上
		t3 = VGet(sx + width / 2.0f, sy + height, 0.0f); // 下の先端
		break;

	case SpikeDir::Left:
		// 左向き（先端が左）
		t1 = VGet(sx + width, sy, 0.0f);// 右上
		t2 = VGet(sx + width, sy + height, 0.0f);// 右下
		t3 = VGet(sx, sy + height / 2.0f, 0.0f);// 左の先端
		break;

	case SpikeDir::Right:
		// 右向き（先端が右）
		t1 = VGet(sx, sy, 0.0f);// 左上
		t2 = VGet(sx, sy + height, 0.0f);// 左下
		t3 = VGet(sx + width, sy + height / 2.0f, 0.0f);// 右の先端
		break;
	}

	if (HitCheck_Circle_Triangle(center, cr, t1, t2, t3))
	{
		
		PlaySoundMem(SpikeSE, DX_PLAYTYPE_BACK);
		ChangeVolumeSoundMem(70, SpikeSE);
		player->ForceDie();
		player->SetDead();
	}
}

void DirectionalSpike::Draw()
{
	// 中心座標（1タイル 64x64 前提）
	float cx = x + width * 0.5f;
	float cy = y + height * 0.5f;

	// 向きごとの回転角度
	double angle = 0.0;

	switch (dir)
	{
	case SpikeDir::Up:
		angle = 0.0; // そのまま
		break;
	case SpikeDir::Right:
		angle = DX_PI / 2.0; // 90度回転
		break;
	case SpikeDir::Down:
		angle = DX_PI; // 180度回転
		break;
	case SpikeDir::Left:
		angle = DX_PI * 3.0 / 2.0; // 270度回転
		break;
	}

	// 拡大率 1.0（そのままの大きさ）
	DrawRotaGraph(
		static_cast<int>(cx),
		static_cast<int>(cy),
		1.0,
		angle,
		hImage,
		TRUE
	);
}
