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
#include <cmath>

void MovingWall::CheckCrushWithPlayer()
{
	if (buildLevel <= 0) return;

	Player* player = FindGameObject<Player>();
	if (!player) return;

	/**/	player->PushByWall(0.1f);
	/**/	return;


	// 壁全体の矩形
	const float wallLeft = x;
	const float wallRight = x + width;
	const float wallBottom = y + blockSize;
	const float wallTop = y - blockSize * (buildLevel - 1);

	// プレイヤー円
	float cx, cy, cr;
	player->GetHitCircle(cx, cy, cr);

	// 円 vs 矩形（最近接点）
	float nearestX = ClampFloat(cx, wallLeft, wallRight);
	float nearestY = ClampFloat(cy, wallTop, wallBottom);

	float ddx = cx - nearestX;
	float ddy = cy - nearestY;

	const bool overlap = (ddx * ddx + ddy * ddy <= cr * cr);
	if (!overlap) return;

	// --------------------------------------------
	// 重要：壁が動いたフレームだけ “必要量” で押し出す
	// --------------------------------------------
	float pushDx = 0.0f;

	const float playerX = player->GetX();
	const float playerW = player->GetW(); // 64想定
	const float halfW = playerW * 0.5f;

	// overlap が true の後（円 vs 矩形）に入れるイメージ
	if (lastMoveDx > 0.0f)
	{
		// 右へ動く壁：円の中心 cx は wallRight + cr 以上にしたい
		float need = (wallRight + cr) - cx;
		if (need > 0.0f)
			player->PushByWall(need + 0.5f); // 少し余裕
	}
	else if (lastMoveDx < 0.0f)
	{
		// 左へ動く壁：cx は wallLeft - cr 以下にしたい
		float need = (wallLeft - cr) - cx;
		if (need < 0.0f)
			player->PushByWall(need - 0.5f);
	}

	const float beforeX = player->GetX();
	if (pushDx != 0.0f)
	{
		player->PushByWall(pushDx);
	}
	const float afterX = player->GetX();
	const float actualDx = afterX - beforeX;

	// 押した後にまだ重なっているか再判定
	player->GetHitCircle(cx, cy, cr);
	nearestX = ClampFloat(cx, wallLeft, wallRight);
	nearestY = ClampFloat(cy, wallTop, wallBottom);
	ddx = cx - nearestX;
	ddy = cy - nearestY;

	bool stillOverlap = (ddx * ddx + ddy * ddy <= cr * cr);
	if (!stillOverlap) return;

	// --------------------------------------------
	// ここが修正ポイント：
	// 「動く壁に押されて、必要量ぶん動けなかった」時だけ死亡（挟まれ）
	// ＝ player がフィールド等に阻まれて押し出しが足りない
	// --------------------------------------------
	const float eps = 0.01f;

	const bool wallMoving = (lastMoveDx != 0.0f);
	const bool triedToPush = (pushDx != 0.0f);

	bool blocked = false;
	if (wallMoving && triedToPush)
	{
		// 必要押し出し量に対して、実際の移動が明らかに足りないなら「押し潰し」
		if (fabsf(actualDx) + eps < fabsf(pushDx))
		{
			blocked = true;
		}
	}

	if (blocked)
	{
		player->ForceDie();
		player->SetDead();
		return;
	}

	// blocked じゃないのに stillOverlap の場合は、丸め/境界の微小ズレが多い
	// → 死亡させず、数回だけ微小押し出しして確実に分離させる
	for (int i = 0; i < 4; ++i)
	{
		player->GetHitCircle(cx, cy, cr);
		nearestX = ClampFloat(cx, wallLeft, wallRight);
		ddx = cx - nearestX;

		// 水平方向に少しだけ押す（壁の外側へ）
		float tiny = 0.0f;
		if (cx < (wallLeft + wallRight) * 0.5f) tiny = -1.0f;
		else                                   tiny = +1.0f;

		player->PushByWall(tiny);

		// 再チェック
		player->GetHitCircle(cx, cy, cr);
		nearestX = ClampFloat(cx, wallLeft, wallRight);
		nearestY = ClampFloat(cy, wallTop, wallBottom);
		ddx = cx - nearestX;
		ddy = cy - nearestY;

		stillOverlap = (ddx * ddx + ddy * ddy <= cr * cr);
		if (!stillOverlap) break;
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