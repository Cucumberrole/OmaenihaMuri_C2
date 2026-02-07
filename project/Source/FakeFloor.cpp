#include "FakeFloor.h"
#include "Player.h"
#include <DxLib.h>

// プレイヤーが「触れたら消える床」
// ここでは AABB(矩形) 同士の当たり判定を行います。
// 注意：元のコードは「重なった時だけ当たり」としていたため、境目にピッタリ接触しただけのケースで
//       どちらの床にも当たっていない扱いになり、消えないことがありました。
//       本修正では「接触も当たり」扱いになるように、分離条件を厳密不等号(<, >)に変更しています。

bool FakeFloor::CheckHitBall(float px, float py, float pw, float ph)
{
	if (vanished_) return false;

	// 接触もヒット扱いにするAABB判定
	// 分離している条件：
	//   プレイヤー右端が床左端より左  (px+pw < x)
	//   プレイヤー左端が床右端より右  (px > x+width)
	//   プレイヤー下端が床上端より上  (py+ph < y)
	//   プレイヤー上端が床下端より下  (py > y+height)
	// のどれかが成立したら「当たっていない」。
	// それ以外＝重なり or 接触 ならヒット。
	const bool hit =
		!(px + pw < x ||
			px > x + width ||
			py + ph < y ||
			py > y + height);

	if (hit)
	{
		vanished_ = true; // 触れたので消す
		return true;
	}
	return false;
}

FakeFloor::FakeFloor(int sx, int sy)
{
	hImage = LoadGraph("data/image/NewBlock.png");
	x = (float)sx;
	y = (float)sy;
}

FakeFloor::~FakeFloor()
{
	DeleteGraph(hImage);
}

void FakeFloor::Update()
{
	// 特に処理なし（必要なら消える演出などをここへ）
}

void FakeFloor::Draw()
{
	if (vanished_) return;
	DrawRectGraph((int)x, (int)y, 0, 0, 64, 64, hImage, TRUE);
}
