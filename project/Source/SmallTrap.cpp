#include <DxLib.h>
#include "SmallTrap.h"
#include "Player.h"

SmallTrap::SmallTrap(int sx,int sy)
{
	SImage = LoadGraph("data/image/Smallhari.png"); // 小さい針画像
	x = static_cast<float>(sx);
	y = static_cast<float>(sy);
	width = 64;
	height = 64;

	offsetY = 0.0f;
	isActive = false;
	isExtended = false;
	moveSpeed = 10.0f; // 上にせり出すスピード

	SetDrawOrder(100); // 描画順序
}

SmallTrap::~SmallTrap()
{
	DeleteGraph(SImage);
}



void SmallTrap::Update()
{
	Player* player = FindGameObject<Player>();
	if (player == nullptr) {
		return;
	}

	float px = player->GetX();
	float py = player->GetY();
	float pw = 64.0f;
	float ph = 64.0f;

	// --- プレイヤーが上のブロックを踏んだら発動 ---
	if (!isActive && !isExtended)
	{
		float trapTop = y - offsetY;
		float trapBottom = trapTop + height;

		if (px + pw > x && px < x + width &&
			py + ph > trapTop && py + ph < trapBottom)
		{
			Activate();
		}
	}

	// --- トラップ上昇アニメーション ---
	if (isActive && !isExtended)
	{
		offsetY += moveSpeed;
		if (offsetY >= 16.0f)
		{
			offsetY = 16.0f;
			isActive = false;
			isExtended = true; // 出きった
		}
	}

	// --- 当たり判定（出ている間だけ） ---
	if (isExtended)
	{
		float baseY = y - offsetY;

		VECTOR tri1 = VGet(x, baseY + height, 0);          // 左下
		VECTOR tri2 = VGet(x + width, baseY + height, 0);  // 右下
		VECTOR tri3 = VGet(x + width / 2, baseY, 0);       // 上の先端

		VECTOR center = VGet(px + pw / 2, py + ph / 2, 0);
		float radius = pw * 0.35f;

		
	}
}


void SmallTrap::Activate()
{
	isActive = true;
}

void SmallTrap::Draw()
{
	// offsetY分だけ上に描画
	DrawRectGraph(
		static_cast<int>(x),
		static_cast<int>(y - offsetY),
		0, 0, width, height,
		SImage,
		TRUE
	);
}

bool SmallTrap::CheckHit(int px, int py, int pw, int ph)
{
	return (px + pw > x && px < x + width &&
		py + ph > y - offsetY && py < y + height);
}


