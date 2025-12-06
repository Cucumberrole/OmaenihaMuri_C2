#include "MovingWall.h"
#include "Field.h"
#include "Player.h"
#include <DxLib.h>
#include <algorithm>

//--------------------------------------
// コンストラクタ
//--------------------------------------
MovingWall::MovingWall(float baseX, float baseY, int dir)
{
	hImage = LoadGraph("data/image/NewBlock.png"); // ブロック画像
	x = baseX;
	y = baseY;        // 一番下のブロックの左上
	width = 64;
	blockSize = 64;
	buildLevel = 0;
	maxLevel = 3;
	buildTimer = 0;
	buildInterval = 10;           // 10フレームごとに 1 段追加

	moveSpeed = 6.0f;         // 横移動速度
	this->dir = (dir >= 0) ? 1 : -1;

	state = State::BUILDING;

	SetDrawOrder(60);
}

//--------------------------------------
// デストラクタ
//--------------------------------------
MovingWall::~MovingWall()
{
	DeleteGraph(hImage);
}

//--------------------------------------
// 1 段追加（積み上げ）
//--------------------------------------
void MovingWall::BuildStep()
{
	if (buildLevel < maxLevel)
	{
		buildLevel++;
		if (buildLevel >= maxLevel)
		{
			// 3 段積み終わったら横移動を開始
			state = State::MOVING;
		}
	}
}

//--------------------------------------
// 横移動（Field のブロックとだけ判定）
//--------------------------------------
void MovingWall::MoveStep()
{
	Field* field = FindGameObject<Field>();
	if (!field) return;

	float oldX = x;
	float dx = moveSpeed * dir;

	// 横に進める
	x += dx;

	if (buildLevel <= 0) return;

	// 壁全体の矩形
	float wallLeft = x;
	float wallRight = x + width;
	float wallBottom = y + blockSize;                      // 一番下ブロックの下端
	float wallTop = y - blockSize * (buildLevel - 1);   // 一番上ブロックの上端

	// 進行方向側のタイルを調べる
	int tileX;
	if (dir > 0)
		tileX = (int)(wallRight) / 64;   // 右側
	else
		tileX = (int)(wallLeft) / 64;    // 左側

	int tileYTop = (int)(wallTop) / 64;
	int tileYBottom = (int)(wallBottom - 1) / 64;

	bool hitWall = false;
	for (int ty = tileYTop; ty <= tileYBottom; ++ty)
	{
		if (field->IsBlock(tileX, ty))
		{
			hitWall = true;
			break;
		}
	}

	if (hitWall)
	{
		// ぶつかったので元の位置に戻して停止
		x = oldX;
		state = State::STOPPED;
	}
}

//--------------------------------------
// プレイヤー押し潰し判定（毎フレーム呼ぶ）
//--------------------------------------
void MovingWall::CheckCrushWithPlayer()
{
	// 壁がまだ無いなら何もしない
	if (buildLevel <= 0) return;

	Player* player = FindGameObject<Player>();
	if (!player) return;

	float cx, cy, cr;
	player->GetHitCircle(cx, cy, cr); // プレイヤーの円当たり判定

	// 壁全体の矩形
	float wallLeft = x;
	float wallRight = x + width;
	float wallBottom = y + blockSize;
	float wallTop = y - blockSize * (buildLevel - 1);

	// 円 vs 矩形の最近接点
	float nearestX = max(wallLeft, min(cx, wallRight));
	float nearestY = max(wallTop, min(cy, wallBottom));

	float ddx = cx - nearestX;
	float ddy = cy - nearestY;

	bool overlap = (ddx * ddx + ddy * ddy <= cr * cr);
	if (!overlap) return; // そもそも当たっていない

	// ここまで来たら「壁にめり込んでいる」

	Field* field = FindGameObject<Field>();
	if (!field) return;

	bool crushed = false;

	if (dir > 0)
	{
		// 右方向に動いている → プレイヤーの右側にブロックがあると挟まれ
		int checkX = (int)(cx + cr) / 64;
		int checkY = (int)(cy) / 64;
		if (field->IsBlock(checkX, checkY))
		{
			crushed = true;
		}
	}
	else
	{
		// 左方向に動いている → プレイヤーの左側にブロックがあると挟まれ
		int checkX = (int)(cx - cr) / 64;
		int checkY = (int)(cy) / 64;
		if (field->IsBlock(checkX, checkY))
		{
			crushed = true;
		}
	}

	if (crushed)
	{
		player->ForceDie();
		player->SetDead();
	}
}

//--------------------------------------
// Update
//--------------------------------------
void MovingWall::Update()
{
	switch (state)
	{
	case State::BUILDING:
		// 一定フレームごとに 1 段追加
		if (buildTimer > 0)
		{
			buildTimer--;
		}
		else
		{
			BuildStep();
			buildTimer = buildInterval;
		}
		break;

	case State::MOVING:
		MoveStep();
		break;

	case State::STOPPED:
		// その場で止まっているだけ
		break;
	}

	// プレイヤーが動いていなくても、毎フレーム押し潰し判定する
	if (state == State::MOVING)
	{
		CheckCrushWithPlayer();
	}
}

//--------------------------------------
// Draw
//--------------------------------------
void MovingWall::Draw()
{
	// buildLevel 段分だけ上に積み上げて描画
	for (int i = 0; i < buildLevel; ++i)
	{
		int drawX = (int)x;
		int drawY = (int)(y - blockSize * i);
		DrawRectGraph(drawX, drawY, 0, 0, blockSize, blockSize, hImage, TRUE);
	}
}

//==================================================================
//  以下は Player.cpp から使う用の「ポイント当たり判定」
//==================================================================

// 右方向の当たり判定（プレイヤーが右に動くとき）
int MovingWall::HitCheckRight(int px, int py)
{
	if (buildLevel <= 0) return 0;

	float wallLeft = x;
	float wallRight = x + width;
	float wallBottom = y + blockSize;
	float wallTop = y - blockSize * (buildLevel - 1);

	// Y方向がかすってなければ当たりなし
	if (py < wallTop || py >= wallBottom) return 0;

	int localX = px - (int)wallLeft;   // 壁左端からの距離
	if (localX >= 0 && localX < width)
	{
		// Field::HitCheckRight と同じ思想：めり込んだ分＋1
		return localX + 1;
	}
	return 0;
}

// 左方向の当たり判定（プレイヤーが左に動くとき）
int MovingWall::HitCheckLeft(int px, int py)
{
	if (buildLevel <= 0) return 0;

	float wallLeft = x;
	float wallRight = x + width;
	float wallBottom = y + blockSize;
	float wallTop = y - blockSize * (buildLevel - 1);

	if (py < wallTop || py >= wallBottom) return 0;

	int localX = px - (int)wallLeft;
	if (localX >= 0 && localX < width)
	{
		return width - localX;
	}
	return 0;
}

// 下方向の当たり判定（プレイヤーが落下してくるとき）
int MovingWall::HitCheckDown(int px, int py)
{
	if (buildLevel <= 0) return 0;

	float wallLeft = x;
	float wallRight = x + width;
	float wallTop = y - blockSize * (buildLevel - 1); // 一番上ブロックの上端

	// X方向にかすってなければ当たりなし
	if (px < wallLeft || px >= wallRight) return 0;

	// 「一番上の面」だけを床として扱う
	int localY = py - (int)wallTop;
	if (localY >= 0 && localY < blockSize)
	{
		return localY + 1;
	}
	return 0;
}

// 上方向の当たり判定（プレイヤーがジャンプしてぶつかるとき）
int MovingWall::HitCheckUp(int px, int py)
{
	if (buildLevel <= 0) return 0;

	float wallLeft = x;
	float wallRight = x + width;
	float wallBottom = y + blockSize;      // 一番下ブロックの下端

	if (px < wallLeft || px >= wallRight) return 0;

	// 「一番下の面」を天井として扱う
	int localY = (int)wallBottom - py;     // 下端からの距離
	if (localY >= 0 && localY < blockSize)
	{
		return localY;
	}
	return 0;
}