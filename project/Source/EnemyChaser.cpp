#include "EnemyChaser.h"
#include "Player.h"
#include <DxLib.h>
#include <cmath>

EnemyChaser::EnemyChaser(float sx, float sy)
{
	animImage = LoadGraph("data/image/すい .png");
	x = sx;
	y = sy;

	speed = 2.0f;  // 敵の移動速度
	size = 48.0f;  // 当たり判定サイズ

	// アニメーション初期化
	animIndex = 0;
	animFrame = 0;
}

EnemyChaser::~EnemyChaser()
{
	DeleteGraph(animImage);
}

void EnemyChaser::Update()
{
	Player* player = FindGameObject<Player>();
	if (!player) return;

	float px = player->GetX();
	float py = player->GetY();

	// --- 追尾処理（シンプル） ---
	float dx = px - x;
	float dy = py - y;
	float dist = sqrtf(dx * dx + dy * dy);

	if (dist > 1.0f)
	{
		x += (dx / dist) * speed;
		y += (dy / dist) * speed;
	}

	// --- プレイヤーと接触したら倒す ---
	float pw = 64, ph = 64;
	bool hit =
		(x < px + pw) &&
		(x + size > px) &&
		(y < py + ph) &&
		(y + size > py);

	if (hit)
	{
		player->ForceDie();
		player->SetDead();
	}

	//  アニメーションの更新
	animFrame = (animFrame + 1) % ANIM_FRAME_INTERVAL;
	if (animFrame == 0)
	{
		animIndex = (animIndex + 1) % ANIM_FRAME_COUNT;     //  アニメーションのコマを更新
	}
}

void EnemyChaser::Draw()
{
	//DrawExtendGraph(
	//	(int)x,
	//	(int)y,
	//	(int)x + 64,
	//	(int)y + 64,
	//	animImage,
	//	TRUE
	//);


	//  アニメーションのコマがTextureAtlasのどこにあるか計算する
	int xRect = (animIndex % ATLAS_WIDTH) * CHARACTER_WIDTH;
	int yRect = (animIndex / ATLAS_WIDTH) * CHARACTER_HEIGHT;

	//  キャラクターをTextureAtlasを使って表示する
	DrawRectGraph(x, y, xRect, yRect, CHARACTER_WIDTH, CHARACTER_HEIGHT, animImage, TRUE, direction);
}
