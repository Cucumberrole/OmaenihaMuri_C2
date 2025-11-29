#include "CeilingSpike.h"
#include "FallingSpike.h"
#include "Player.h"
#include <DxLib.h>

CeilingSpike::CeilingSpike(int sx, int sy)
{
	hImage = LoadGraph("data/image/hariBottom.png");
	x = sx;
	y = sy;
	triggered = false;
}

CeilingSpike::~CeilingSpike()
{
	DeleteGraph(hImage);
}

void CeilingSpike::Update()
{
	if (triggered) return;

	Player* p = FindGameObject<Player>();
	if (!p) return;

	float px = p->GetX();
	float py = p->GetY();

	// ↓ プレイヤーが「真下を通った瞬間」を判定
	bool under =
		px + 64 > x &&
		px < x + 64 &&
		py > y;

	if (under)
	{
		triggered = true;
		SpawnFallingSpike();
	}
}

void CeilingSpike::SpawnFallingSpike()
{
	// プレイヤーの後ろに落ちる位置 = 針の元の位置そのまま
	new FallingSpike(x, y);
}

void CeilingSpike::Draw()
{
	DrawGraph((int)x, (int)y, hImage, TRUE);
}