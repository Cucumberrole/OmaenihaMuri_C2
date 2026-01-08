#include "Player.h"
#include "Field.h"
#include "FallingFloor.h"
#include "Trap.h"
#include "GameOver.h"
#include "VanishingFloor.h"
#include "MovingWall.h"
#include "Common.h"
#include "../Library/Trigger.h"
#include <assert.h>

// --- 定数 ---
static const float Gravity = 0.4f;  // 重力加速度
static const float V0 = -10.0f;     // ジャンプ初速度（上方向）

//--------------------------------------
// デフォルトコンストラクタ
//--------------------------------------
Player::Player()
{
	hImage = LoadGraph("data/image/OMAEwalk.png");
	assert(hImage != -1);

	x = 200;
	y = 500;
	velocity = 0;
	onGround = false;

	jumpcount = 0;
	Maxjumpcount = 1;

	isDead = false;

	animIndex = 0;
	animFrame = 0;
	frip = true;

	hp = 0;

	// 円当たり判定の半径
	hitRadius = 22.0f;

	hDeadUpImage = LoadGraph("data/image/お前ガチでちんこ.png");
	hDeadFallImage = LoadGraph("data/image/お前ガチでまんこ.png");
	assert(hDeadUpImage != -1);
	assert(hDeadFallImage != -1);

	deathState = DeathState::None;
	deathAnimEnd = false;
}

//--------------------------------------
// 座標指定コンストラクタ
//--------------------------------------
Player::Player(int sx, int sy)
{
	hImage = LoadGraph("data/image/OMAEwalk.png");
	assert(hImage != -1);

	x = (float)sx;
	y = (float)sy;
	velocity = 0;
	onGround = false;

	jumpcount = 0;
	Maxjumpcount = 1;

	isDead = false;

	animIndex = 0;
	animFrame = 0;
	frip = false;

	hp = 0;

	// 半径設定
	hitRadius = 22.0f;

	SetDrawOrder(0);
}

//--------------------------------------
// デストラクタ
//--------------------------------------
Player::~Player()
{
	DeleteGraph(hImage);
	if (hImage != -1) DeleteGraph(hImage);
	if (hDeadUpImage != -1) DeleteGraph(hDeadUpImage);
	if (hDeadFallImage != -1) DeleteGraph(hDeadFallImage);
}

//--------------------------------------
// 座標取得用
//--------------------------------------

// 円形当たり判定を取得
void Player::GetHitCircle(float& outX, float& outY, float& outRadius) const
{
	outX = x + CHARACTER_WIDTH / 2.0f;   // 中心 X
	outY = y + CHARACTER_HEIGHT / 2.0f;  // 中心 Y
	outRadius = hitRadius;
}

void Player::PushByWall(float dx)
{
	if (dx == 0.0f) return;

	Field* field = FindGameObject<Field>();
	if (!field) { x += dx; return; }

	// おす
	x += dx;

	// 押された後にフィールドへめり込んだら押し戻す
	int push = 0;

	if (dx > 0)
	{
		int push1 = field->HitCheckRight((int)(x + 63), (int)(y + 5));
		int push2 = field->HitCheckRight((int)(x + 63), (int)(y + 58));
		push = max(push1, push2);

		if (push > 0) x -= (float)push;
	}
	else
	{
		int push1 = field->HitCheckLeft((int)(x + 0), (int)(y + 5));
		int push2 = field->HitCheckLeft((int)(x + 0), (int)(y + 58));
		push = max(push1, push2);

		if (push > 0) x += (float)push;
	}
}

bool Player::IsdeathAnimEnd() const
{
	return deathAnimEnd;
}



//--------------------------------------
// Update()
//--------------------------------------
void Player::Update()
{
	// --- 死亡していたら完全固定 ---
	if (isDead)
	{
		y += velocity;
		velocity += Gravity;

		if (deathState == DeathState::Up && velocity >= 0.0f)
		{
			deathState = DeathState::Fall;
		}

		int sw = 0, sh = 0;
		GetDrawScreenSize(&sw, &sh);

		// when completely off the bottom -> end
		if (y > sh + 100)
		{
			deathAnimEnd = true;
		}

		return; // no input / no collision / no normal update
	}

	Field* field = FindGameObject<Field>();
	if (!field) return;

	// --- 地面にいるときはジャンプ回数をリセット ---
	if (onGround && jumpcount < Maxjumpcount) {
		jumpcount += 1;
	}

	//========================================================
	// 横移動（移動 → 押し戻し）
	//========================================================
	int moveX = 0;

	if (CheckHitKey(KEY_INPUT_D)) {
		moveX = WALK_SPEED;
		frip = false;
	}
	else if (CheckHitKey(KEY_INPUT_A)) {
		moveX = -WALK_SPEED;
		frip = true;
	}

	// 歩行アニメーション（常時アニメしたいなら moveX 判定を外す）
	if (moveX != 0) {
		animFrame = (animFrame + 1) % ANIM_FRAME_INTERVAL;
		if (animFrame == 0) {
			animIndex = (animIndex + 1) % ANIM_FRAME_COUNT;
		}
	}

	// まず移動
	if (moveX != 0)
	{
		x += (float)moveX;

		// 壁押し戻し量
		int push = 0;

		// --- Field 判定 ---
		if (moveX > 0) {
			// 右端は「63」にする
			int push1 = field->HitCheckRight((int)(x + 63), (int)(y + 5));
			int push2 = field->HitCheckRight((int)(x + 63), (int)(y + 58));
			push = max(push1, push2);
		}
		else {
			int push1 = field->HitCheckLeft((int)(x + 0), (int)(y + 5));
			int push2 = field->HitCheckLeft((int)(x + 0), (int)(y + 58));
			push = max(push1, push2);
		}

		// --- FallingFloor 判定 ---
		auto floors = FindGameObjects<FallingFloor>();
		for (auto f : floors) {
			if (moveX > 0) {
				int p1 = f->HitCheckRight((int)(x + 63), (int)(y + 1));
				int p2 = f->HitCheckRight((int)(x + 63), (int)(y + 62));
				push = max(push, max(p1, p2));
			}
			else {
				int p1 = f->HitCheckLeft((int)(x + 0), (int)(y + 1));
				int p2 = f->HitCheckLeft((int)(x + 0), (int)(y + 62));
				push = max(push, max(p1, p2));
			}
		}

		// --- VanishingFloor 判定（あるなら） ---
		auto vFloors = FindGameObjects<VanishingFloor>();
		for (auto vf : vFloors) {
			if (!vf->IsActive()) continue;

			if (moveX > 0) {
				int p1 = vf->HitCheckRight((int)(x + 63), (int)(y + 1));
				int p2 = vf->HitCheckRight((int)(x + 63), (int)(y + 62));
				push = max(push, max(p1, p2));
			}
			else {
				int p1 = vf->HitCheckLeft((int)(x + 0), (int)(y + 1));
				int p2 = vf->HitCheckLeft((int)(x + 0), (int)(y + 62));
				push = max(push, max(p1, p2));
			}
		}

		// 押し戻し
		if (push > 0) {
			if (moveX > 0) x -= (float)push;
			else           x += (float)push;
		}
	}

	//========================================================
	// ジャンプ処理
	//========================================================
	if (onGround) {
		if (KeyTrigger::CheckTrigger(KEY_INPUT_SPACE)) {
			velocity = V0;
			onGround = false;
		}
	}

	// 二段ジャンプ
	if (!onGround && jumpcount == Maxjumpcount) {
		if (KeyTrigger::CheckTrigger(KEY_INPUT_SPACE)) {
			jumpcount -= 1;
			velocity = V0;
		}
	}

	//========================================================
	// 重力
	//========================================================
	y += velocity;
	velocity += Gravity;

	//========================================================
	// 縦方向当たり判定（床／天井）
	//========================================================
	if (velocity >= 0) {
		int push1 = field->HitCheckDown((int)(x + 5), (int)(y + 64));
		int push2 = field->HitCheckDown((int)(x + 58), (int)(y + 64));
		int push = max(push1, push2);

		auto floors = FindGameObjects<FallingFloor>();
		for (auto f : floors) {
			int p1 = f->HitCheckDown((int)(x + 1), (int)(y + 64));
			int p2 = f->HitCheckDown((int)(x + 62), (int)(y + 64));
			push = max(push, max(p1, p2));
		}

		auto vFloors = FindGameObjects<VanishingFloor>();
		for (auto vf : vFloors) {
			if (!vf->IsActive()) continue;
			int p1 = vf->HitCheckDown((int)(x + 1), (int)(y + 64));
			int p2 = vf->HitCheckDown((int)(x + 62), (int)(y + 64));
			push = max(push, max(p1, p2));
		}

		if (push > 0) {
			y -= (float)(push - 1);
			velocity = 0;
			onGround = true;
		}
		else {
			onGround = false;
		}
	}
	else {
		int push1 = field->HitCheckUp((int)(x + 5), (int)(y + 1));
		int push2 = field->HitCheckUp((int)(x + 58), (int)(y + 1));
		int push = max(push1, push2);

		auto floors = FindGameObjects<FallingFloor>();
		for (auto f : floors) {
			int p1 = f->HitCheckUp((int)(x + 1), (int)(y + 0));
			int p2 = f->HitCheckUp((int)(x + 62), (int)(y + 0));
			push = max(push, max(p1, p2));
		}

		auto vFloors = FindGameObjects<VanishingFloor>();
		for (auto vf : vFloors) {
			if (!vf->IsActive()) continue;
			int p1 = vf->HitCheckUp((int)(x + 1), (int)(y + 0));
			int p2 = vf->HitCheckUp((int)(x + 62), (int)(y + 0));
			push = max(push, max(p1, p2));
		}

		if (push > 0) {
			y += (float)push;
			velocity = 0;
		}
	}

	//========================================================
	// 土管の判定（上に乗った時だけワープ）
	//========================================================
	if (field)
	{
		const float pw = 64.0f;
		const float ph = 64.0f;

		float px = x;
		float py = y;

		float footY = py + ph;

		for (int i = 0; i < (int)field->pipesIn.size(); i++)
		{
			POINT in = field->pipesIn[i];

			float pipeLeft = (float)in.x;
			float pipeRight = (float)in.x + 64.0f;
			float pipeTop = (float)in.y;

			bool overlapX =
				(px + pw > pipeLeft) &&
				(px < pipeRight);

			bool onPipeTop = (footY >= pipeTop - 2.0f) && (footY <= pipeTop + 16.0f);

			if (overlapX && onPipeTop)
			{
				if (!field->pipesOut.empty())
				{
					POINT out = field->pipesOut[i % field->pipesOut.size()];
					x = (float)out.x;
					y = (float)out.y + ph;
				}
				return;
			}
		}
	}
}



//--------------------------------------
// Draw()
//--------------------------------------
void Player::Draw()
{
	if (isDead)
	{
		if (deathState == DeathState::Up)
		{
			DrawGraph((int)x, (int)y, hDeadUpImage, TRUE);
		}
		else
		{
			DrawGraph((int)x, (int)y, hDeadFallImage, TRUE);
		}
		return;
	}

	// 現在のスプライトシート上での位置
	int xRect = (animIndex % ATLAS_WIDTH) * CHARACTER_WIDTH;
	int yRect = (animIndex / ATLAS_WIDTH) * CHARACTER_HEIGHT;

	DrawRotaGraph(0, 0, 0.015625, 1, hDeadUpImage, TRUE, 0, 0);
	DrawRotaGraph(0, 0, 0.015625, 1, hDeadFallImage, TRUE, 0, 0);

	DrawRectGraph(
		(int)x, (int)y,
		xRect, yRect,
		CHARACTER_WIDTH, CHARACTER_HEIGHT,
		hImage,
		TRUE,
		frip
	);

	// デバッグ用：当たり判定円
	float cx, cy, r;
	GetHitCircle(cx, cy, r);
	DrawCircle((int)cx, (int)cy, (int)r, GetColor(0, 255, 0), FALSE);
}

void Player::ForceDie()
{
	if (Common::GetInstance() && Common::GetInstance()->invincible)
	{
		return;
	}

	if (isDead) return;        // or: if (IsDead()) return;

	onGround = false;

	// Death animation start
	isDead = true;
	deathAnimEnd = false;
	deathState = DeathState::Up;
	velocity = V0;
}
