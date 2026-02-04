#include <DxLib.h>
#include "Trap.h"
#include "Player.h"
#include "Collision.h"

#include "SoundCache.h"

//--------------------------------------
// コンストラクタ
//--------------------------------------
Trap::Trap(int sx, int sy)
{
	hImage = LoadGraph("data/image/hari.png"); // 針画像
	x = sx;
	y = sy;
	width = 64;
	height = 64;

	offsetY = 0.0f;
	isActive = false;
	isExtended = false;
	moveSpeed = 10.0f; // 上にせり出すスピード

	SetDrawOrder(100);  // 描画順序
	SpikeSE = SoundCache::Get("data/BGM/spikeDeath.mp3");
}

//--------------------------------------
// デストラクタ
//--------------------------------------
Trap::~Trap()
{
	DeleteGraph(hImage);
	StopSoundMem(SpikeSE);
}

//--------------------------------------
// Update
//--------------------------------------
void Trap::Update()
{
	Player* player = FindGameObject<Player>();
	if (player == nullptr) {
		return;
	}

	// プレイヤーの「円当たり判定」を取得
	float cx, cy, cr;
	player->GetHitCircle(cx, cy, cr);

	//----------------------------------
	// プレイヤーが「上のブロック」を踏んだら発動
	// 円＆矩形で判定中
	//----------------------------------
	if (!isActive && !isExtended)
	{
		// 針の真上 1マス分
		float triggerLeft = x;
		float triggerRight = x + width;
		float triggerTop = y - height;
		float triggerBottom = y;

		float marginX = 0.0f; // 少し左右にマージンを足したいならここを調整
		triggerLeft -= marginX;
		triggerRight += marginX;

		// 円 vs AABB 最近接点をとる
		float nearestX = cx;
		if (nearestX < triggerLeft)  nearestX = triggerLeft;
		else if (nearestX > triggerRight) nearestX = triggerRight;

		float nearestY = cy;
		if (nearestY < triggerTop)   nearestY = triggerTop;
		else if (nearestY > triggerBottom) nearestY = triggerBottom;

		float dx = cx - nearestX;
		float dy = cy - nearestY;

		if (dx * dx + dy * dy <= cr * cr)
		{
			// 左右・斜め・落下中どこから来ても円が範囲に入れば発動
			Activate();
		}
	}

	//----------------------------------
	// トラップ上昇アニメーション
	//----------------------------------
	if (isActive && !isExtended)
	{
		offsetY += moveSpeed;
		if (offsetY >= height)
		{
			offsetY = (float)height;
			isActive = false;
			isExtended = true;   // 出きった
		}
	}

	//----------------------------------
	// 当たり判定（出ている間だけ）
	//----------------------------------
	if (isExtended)
	{
		float baseY = y - offsetY;

		// 三角形の頂点（画面上の座標）
		VECTOR tri1 = VGet(x, baseY + height, 0); // 左下
		VECTOR tri2 = VGet(x + width, baseY + height, 0); // 右下
		VECTOR tri3 = VGet(x + width / 2, baseY, 0); // 上の先端

		VECTOR center = VGet(cx, cy, 0);
		float  radius = cr;

		if (HitCheck_Circle_Triangle(center, radius, tri1, tri2, tri3))
		{
			PlaySoundMem(SpikeSE, DX_PLAYTYPE_BACK);
			player->ForceDie();
			player->SetDead();
		}
	}
}

//--------------------------------------
// Activate
//--------------------------------------
void Trap::Activate()
{
	isActive = true;
}

//--------------------------------------
// Draw
//--------------------------------------
void Trap::Draw()
{
	// offsetY 分だけ上に描画
	DrawRectGraph(
		static_cast<int>(x),
		static_cast<int>(y - offsetY),
		0, 0, width, height,
		hImage, TRUE);
}