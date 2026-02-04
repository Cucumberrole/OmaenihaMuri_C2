#include "EnemyChaser.h"
#include "Player.h"
#include "SoundCache.h"
#include <DxLib.h>

EnemyChaser::EnemyChaser(float sx, float sy)
{
	hImage = LoadGraph("data/image/sui.png"); // 1280×640

	x = sx;
	y = sy;

	speed = 1.5f;

	animIndex = 0;
	animFrame = 0;

	SetDrawOrder(10);
	SpikeSE = SoundCache::Get("data/BGM/spikeDeath.mp3");
}

EnemyChaser::~EnemyChaser()
{
	DeleteGraph(hImage);
	StopSoundMem(SpikeSE);
}

void EnemyChaser::Update()
{
	// アニメーション（常時）
	animFrame++;
	if (animFrame >= ANIM_FRAME_INTERVAL)
	{
		animFrame = 0;
		animIndex = (animIndex + 1) % 2;
	}

	// プレイヤー追尾処理
	Player* player = FindGameObject<Player>();
	if (player)
	{
		float px = player->GetX();
		float py = player->GetY();

		if (px < x) x -= speed;
		if (px > x) x += speed;
		if (py < y) y -= speed;
		if (py > y) y += speed;
	}

	// プレイヤー接触で死亡
	if (player)
	{
		float px = player->GetX();
		float py = player->GetY();
		float pw = 64;   // プレイヤーの幅
		float ph = 64;   // プレイヤーの高さ

		// 敵のサイズ（64×64想定）必要なら変更OK
		float ew = 64;
		float eh = 64;

		// --- AABB（四角形同士）当たり判定 ---
		bool hit =
			x < px + pw &&
			x + ew > px &&
			y < py + ph &&
			y + eh > py;

		if (hit)
		{
			PlaySoundMem(SpikeSE, DX_PLAYTYPE_BACK);
			player->ForceDie();  // プレイヤーを即死扱いにする関数
			player->SetDead();   // 動きを止める
		}
	}
}

void EnemyChaser::Draw()
{
	int srcX = animIndex * CHARACTER_WIDTH;
	int srcY = 0;

	DrawRectExtendGraph(
		(int)x, (int)y,               // 描画左上
		(int)x + 64, (int)y + 64,     // 描画右下（64×64に縮小）
		srcX, srcY,                   // 切り出し位置
		CHARACTER_WIDTH,              // 元コマ幅 640
		CHARACTER_HEIGHT,             // 元コマ高さ 640
		hImage,
		TRUE
	);
}