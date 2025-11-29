#include "Boss.h"
#include "Player.h"
#include "Field.h"
#include <DxLib.h>

Boss::Boss(int sx, int sy)
{
	hImage = LoadGraph("data/image/すい2.png");

	x = sx;
	y = sy;

	vx = 0;
	vy = 0;
	gravity = 0.5f;

	hp = 30;  // ボスのHP

	state = STATE_IDLE;
	stateTimer = 60;
	facingLeft = true;

	SetDrawOrder(5);
}

Boss::~Boss()
{
	DeleteGraph(hImage);
}

void Boss::ChangeState(State newState)
{
	state = newState;
	stateTimer = 60;  // 基本クールタイム
}

void Boss::Update()
{
	Player* player = FindGameObject<Player>();
	Field* field = FindGameObject<Field>();

	if (!player || !field) return;

	// --- AI制御 ---
	switch (state)
	{
	case STATE_IDLE:        DoIdle(); break;
	case STATE_WALK:        DoWalk(); break;
	case STATE_CHASE:       DoChase(); break;
	case STATE_JUMP_ATTACK: DoJumpAttack(); break;
	case STATE_COOLDOWN:    DoCooldown(); break;
	}

	// --- マップ当たり判定 ---
	CollisionWithMap();

	// --- プレイヤーとの当たり判定 ---
	AttackCheck();
}

void Boss::DoIdle()
{
	vx = 0;

	if (--stateTimer <= 0)
		ChangeState(STATE_WALK);
}

void Boss::DoWalk()
{
	vx = facingLeft ? -2.0f : 2.0f;

	// たまに向きを変える
	if (rand() % 100 == 0)
		facingLeft = !facingLeft;

	if (--stateTimer <= 0)
		ChangeState(STATE_CHASE);
}

void Boss::DoChase()
{
	Player* player = FindGameObject<Player>();
	if (!player) return;

	if (player->GetX() < x)
	{
		vx = -3.0f;
		facingLeft = true;
	}
	else
	{
		vx = 3.0f;
		facingLeft = false;
	}

	if (--stateTimer <= 0)
		ChangeState(STATE_JUMP_ATTACK);
}

void Boss::DoJumpAttack()
{
	if (stateTimer == 60)
	{
		// 初回だけジャンプの初速を入れる
		vy = -15;
		vx = facingLeft ? -5.0f : 5.0f;
	}

	if (--stateTimer <= 0)
		ChangeState(STATE_COOLDOWN);
}

void Boss::DoCooldown()
{
	vx = 0;

	if (--stateTimer <= 0)
		ChangeState(STATE_IDLE);
}

void Boss::CollisionWithMap()
{
	Field* field = FindGameObject<Field>();
	if (!field) return;

	// 重力
	vy += gravity;
	y += vy;

	// --- 下方向 ---
	int tx1 = (int)(x + 20) / 64;
	int tx2 = (int)(x + 236) / 64;
	int ty = (int)(y + 256) / 64;

	if (field->IsBlock(tx1, ty) || field->IsBlock(tx2, ty))
	{
		y = ty * 64 - 256;
		vy = 0;
	}

	// 横移動
	x += vx;
}

void Boss::AttackCheck()
{
	Player* player = FindGameObject<Player>();
	if (!player) return;

	float px = player->GetX();
	float py = player->GetY();

	// 体当たり判定（256×256）
	bool hit =
		px < x + 256 &&
		px + 64 > x &&
		py < y + 256 &&
		py + 64 > y;

	if (hit)
	{
		player->ForceDie();
		player->SetDead();
	}
}

void Boss::Draw()
{
	DrawExtendGraph(
		(int)x, (int)y,
		(int)x + 256, (int)y + 256,
		hImage,
		TRUE
	);

	SetFontSize(30);
	int h = GetFontSize();

	DrawFormatString(0, 200 + h * 0, GetColor(255, 255, 255), "BossX: %.2f", x);
	DrawFormatString(0, 200 + h * 1, GetColor(255, 255, 255), "BossY: %.2f", y);
	DrawFormatString(0, 200 + h * 2, GetColor(255, 255, 255), "BossHP: %d", hp);
}