#include "MovingSmallTrap.h"
#include "Player.h"
#include "Telop.h"
#include "Collision.h"
#include "SoundCache.h"

MovingSmallTrap::MovingSmallTrap(float sx, float sy)
{
	SImage = LoadGraph("data/image/Smallhari.png"); // 小さい針画像
	x = sx;
	y = sy;
	SpikeSE = SoundCache::Get("data/BGM/spikeDeath.mp3");
}

MovingSmallTrap::~MovingSmallTrap()
{
	StopSoundMem(SpikeSE);
}

void MovingSmallTrap::Update()
{
	Player* player = FindGameObject<Player>();
	Telop* telop = FindGameObject<Telop>();
	if (!player) return;

	float cx, cy, cr;
	player->GetHitCircle(cx, cy, cr);
	VECTOR center = VGet(cx, cy, 0.0f);

	int trapTileX = static_cast<int>((x + width * 0.5f) / tile);
	int trapTileY = static_cast<int>((y + height * 0.5f) / tile);
	int playerTileX = static_cast<int>(cx / tile);
	int playerTileY = static_cast<int>(cy / tile);

	if (!moved && playerTileY == trapTileY) // 上下のマスじゃないときだけ
	{
		if (playerTileX == trapTileX - 1)
		{
			x -= tile;
			moved = true;
		}
		else if (playerTileX == trapTileX + 1)
		{
			x += tile;
			moved = true;
		}
	}



	// トラップの当たり判定(三角)
	float sx = x;
	float sy = y;
	VECTOR t1{}, t2{}, t3{};
	t1 = VGet(sx, sy + height, 0.0f); // 左下
	t2 = VGet(sx + width, sy + height, 0.0f); // 右下
	t3 = VGet(sx + width / 2.0f, sy, 0.0f); // 上の先端

	if (HitCheck_Circle_Triangle(center, cr, t1, t2, t3))
	{

		player->ForceDie();
		player->SetDead();
		PlaySoundMem(SpikeSE, DX_PLAYTYPE_BACK);
		if (y < 384) {
			telop->TouchedTrap1 = true;
		}
	}


}

void MovingSmallTrap::Draw()
{
	// 中心座標（1タイル 64x64 前提）
	float cx = x + width * 0.5f;
	float cy = y + height * 0.5f;
	// 拡大率 1.0（そのままの大きさ）
	DrawRotaGraph(static_cast<int>(cx), static_cast<int>(cy), 1.0, 0, SImage, TRUE);
}
