#include "MovingWall.h"
#include "Field.h"
#include "Player.h"
#include <DxLib.h>

// clamp を自前で（min/max マクロ問題を避ける）
static float ClampFloat(float v, float a, float b)
{
	if (v < a) return a;
	if (v > b) return b;
	return v;
}

//--------------------------------------
// コンストラクタ
//--------------------------------------
MovingWall::MovingWall(float baseX, float baseY, int dir)
{
	hImage = LoadGraph("data/image/NewBlock.png");
	x = baseX;
	y = baseY;

	width = 64;
	blockSize = 64;
	buildLevel = 0;
	maxLevel = 3;
	buildTimer = 0;
	buildInterval = 10;

	moveSpeed = 6.0f;
	this->dir = (dir >= 0) ? 1 : -1;

	state = State::BUILDING;
	lastMoveDx = 0.0f;

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
	if (!field) { lastMoveDx = 0.0f; return; }

	float oldX = x;
	float dx = moveSpeed * dir;

	// 横に進める
	x += dx;

	if (buildLevel <= 0) { lastMoveDx = 0.0f; return; }

	// 壁全体の矩形
	float wallLeft = x;
	float wallRight = x + width;
	float wallBottom = y + blockSize;
	float wallTop = y - blockSize * (buildLevel - 1);

	// 進行方向側のタイルを調べる
	int tileX = (dir > 0) ? (int)(wallRight) / 64 : (int)(wallLeft) / 64;

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

	// 実際に動いた量（止まったら 0 ）
	lastMoveDx = x - oldX;
}

//--------------------------------------
// プレイヤー押し出し + 挟まれ死亡（毎フレーム）
//--------------------------------------
void MovingWall::CheckCrushWithPlayer()
{
	if (buildLevel <= 0) return;

	Player* player = FindGameObject<Player>();
	if (!player) return;

	Field* field = FindGameObject<Field>();
	if (!field) return;

	// 壁全体の矩形
	float wallLeft = x;
	float wallRight = x + width;
	float wallBottom = y + blockSize;
	float wallTop = y - blockSize * (buildLevel - 1);

	// プレイヤー円
	float cx, cy, cr;
	player->GetHitCircle(cx, cy, cr);

	// 円 vs 矩形（最近接点）
	float nearestX = ClampFloat(cx, wallLeft, wallRight);
	float nearestY = ClampFloat(cy, wallTop, wallBottom);

	float ddx = cx - nearestX;
	float ddy = cy - nearestY;

	bool overlap = (ddx * ddx + ddy * ddy <= cr * cr);
	if (!overlap) return;

	// ===========================
	// ここから「押す」処理が本体
	// ===========================
	// 壁がこのフレームで動いていないなら押せない（停止中など）
	if (lastMoveDx != 0.0f)
	{
		// 壁の移動量ぶんプレイヤーを押す
		// ※ Player に PushByWall(dx) を追加してください
		player->PushByWall(lastMoveDx);
	}

	// 押した後もまだ重なってるなら「押し切れない＝挟まれ」
	player->GetHitCircle(cx, cy, cr);
	nearestX = ClampFloat(cx, wallLeft, wallRight);
	nearestY = ClampFloat(cy, wallTop, wallBottom);

	ddx = cx - nearestX;
	ddy = cy - nearestY;

	bool stillOverlap = (ddx * ddx + ddy * ddy <= cr * cr);
	if (stillOverlap)
	{

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
		if (buildTimer > 0)
		{
			buildTimer--;
		}
		else
		{
			BuildStep();
			buildTimer = buildInterval;
		}
		lastMoveDx = 0.0f;
		break;

	case State::MOVING:
		MoveStep();
		break;

	case State::STOPPED:
		lastMoveDx = 0.0f;
		break;
	}

	CheckCrushWithPlayer();
}

//--------------------------------------
// Draw
//--------------------------------------
void MovingWall::Draw()
{
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
	float wallBottom = y + blockSize;
	float wallTop = y - blockSize * (buildLevel - 1);

	if (py < wallTop || py >= wallBottom) return 0;

	int localX = px - (int)wallLeft;
	if (localX >= 0 && localX < width)
	{
		return localX + 1;
	}
	return 0;
}

// 左方向の当たり判定（プレイヤーが左に動くとき）
int MovingWall::HitCheckLeft(int px, int py)
{
	if (buildLevel <= 0) return 0;

	float wallLeft = x;
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

	if (px < wallLeft || px >= wallRight) return 0;

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
	float wallBottom = y + blockSize; // 一番下ブロックの下端

	if (px < wallLeft || px >= wallRight) return 0;

	int localY = (int)wallBottom - py;
	if (localY >= 0 && localY < blockSize)
	{
		return localY;
	}
	return 0;
}