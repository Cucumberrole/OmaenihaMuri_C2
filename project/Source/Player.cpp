#include "Player.h"
#include "Field.h"
#include "FallingFloor.h"
#include "Trap.h"
#include "GameOver.h"
#include "VanishingFloor.h"
#include "MovingWall.h"
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
}

//--------------------------------------
// 座標指定コンストラクタ
//--------------------------------------
Player::Player(int sx, int sy)
{
	hImage = LoadGraph("data/image/OMAEwalk.png");
	assert(hImage != -1);

	x = sx;
	y = sy;
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
}

//--------------------------------------
// 座標取得用
//--------------------------------------
float Player::GetX() const
{
	return x;
}

float Player::GetY() const
{
	return y;
}

// 円形当たり判定を取得
void Player::GetHitCircle(float& outX, float& outY, float& outRadius) const
{
	outX = x + CHARACTER_WIDTH / 2.0f;  // 中心 X
	outY = y + CHARACTER_HEIGHT / 2.0f;  // 中心 Y
	outRadius = hitRadius;
}

//--------------------------------------
// Update()
//--------------------------------------
void Player::Update()
{
	// --- 死亡していたら完全固定 ---
	if (isDead) {
		return;  // 動きを何も更新しない
	}

	// --- 地面にいるときはジャンプ回数をリセット ---
	if (onGround && jumpcount < Maxjumpcount) {
		jumpcount += 1;
	}

	int moveX = 0;
	//--------------------------------------
	// 右移動（Dキー）
	//--------------------------------------
	if (CheckHitKey(KEY_INPUT_D)) {
		moveX = WALK_SPEED;
		frip = false;

		// --- Field 判定 ---
		Field* field = FindGameObject<Field>();
		int push1 = field->HitCheckRight(x + 64, y + 5);
		int push2 = field->HitCheckRight(x + 64, y + 58);
		int push = max(push1, push2);



		// --- 落下床 判定（複数床対応） ---
		auto floors = FindGameObjects<FallingFloor>();
		for (auto f : floors) {
			int p1 = f->HitCheckRight(x + 64, y + 1);
			int p2 = f->HitCheckRight(x + 64, y + 62);
			push = max(push, max(p1, p2));
		}


		x -= push;
	}

	//--------------------------------------
	// 左移動（Aキー）
	//--------------------------------------
	if (CheckHitKey(KEY_INPUT_A)) {
		moveX = -WALK_SPEED;
		frip = true;

		Field* field = FindGameObject<Field>();
		int push1 = field->HitCheckLeft(x, y + 5);
		int push2 = field->HitCheckLeft(x, y + 58);
		int push = max(push1, push2);



		auto floors = FindGameObjects<FallingFloor>();
		for (auto f : floors) {
			int p1 = f->HitCheckLeft(x, y + 1);
			int p2 = f->HitCheckLeft(x, y + 62);
			push = max(push, max(p1, p2));
		}

		x += push;
	}

	if (CheckHitKey(KEY_INPUT_0)) {
	}

	//--------------------------------------
	// 歩行アニメーション更新
	//--------------------------------------
	if (moveX != 0) {
		// 一定間隔で次のコマに切り替える
		animFrame = (animFrame + 1) % ANIM_FRAME_INTERVAL;
		if (animFrame == 0) {
			animIndex = (animIndex + 1) % ANIM_FRAME_COUNT;
		}

		// 実際の位置を更新
		x += moveX;
	}

	//マップクラスの取得
	Field* field = FindGameObject<Field>();

	//--------------------------------------
	// ジャンプ処理（接地中）
	//--------------------------------------
	if (onGround) {
		if (KeyTrigger::CheckTrigger(KEY_INPUT_SPACE)) {
			velocity = V0;     // 上方向に加速
			onGround = false;  // 空中へ
		}
	}

	//--------------------------------------
	// 二段ジャンプ処理
	//--------------------------------------
	if (!onGround && jumpcount == Maxjumpcount) {
		if (KeyTrigger::CheckTrigger(KEY_INPUT_SPACE)) {
			jumpcount -= 1;
			velocity = V0;
		}
	}

	//--------------------------------------
	// 重力適用
	//--------------------------------------
	y += velocity;
	velocity += Gravity;

	//--------------------------------------
	// 当たり判定（下方向：床）
	//--------------------------------------

	if (velocity >= 0) { // 落下中
		int push1 = field->HitCheckDown(x + 5, y + 64);
		int push2 = field->HitCheckDown(x + 58, y + 64);
		int push = max(push1, push2);

		// --- 落下床との当たり判定 ---
		auto floors = FindGameObjects<FallingFloor>();
		for (auto f : floors) {
			int p1 = f->HitCheckDown(x + 1, y + 64);
			int p2 = f->HitCheckDown(x + 62, y + 64);
			push = max(push, max(p1, p2));
		}

		if (push > 0) {
			y -= push - 1;
			velocity = 0;
			onGround = true;
		}
		else {
			onGround = false;
		}
	}
	else { // 上昇中
		int push1 = field->HitCheckUp(x + 5, y + 1);
		int push2 = field->HitCheckUp(x + 58, y + 1);
		int push = max(push1, push2);

		// --- 落下床との当たり判定 ---
		auto floors = FindGameObjects<FallingFloor>();
		for (auto f : floors) {
			int p1 = f->HitCheckUp(x + 1, y);
			int p2 = f->HitCheckUp(x + 62, y);
			push = max(push, max(p1, p2));
		}

		if (push > 0) {
			y += push;
			velocity = 0;
		}
	}

	//--------------------------------------
	// 土管の判定（上に乗った時だけワープ）
	//--------------------------------------
	if (field)
	{
		// プレイヤーの大きさ（64x64前提）
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
			float pipeBottom = (float)in.y + 64.0f;

			// 横方向は土管と重なっているか
			bool overlapX =
				(px + pw > pipeLeft) &&
				(px < pipeRight);

			// 足が「土管の天面付近」にあるかどうか
			bool onPipeTop = (footY >= pipeTop - 2.0f) && (footY <= pipeTop + 16.0f);

			if (overlapX && onPipeTop)
			{
				// 出口があるかチェック
				if (!field->pipesOut.empty())
				{
					POINT out = field->pipesOut[i % field->pipesOut.size()];

					// 土管出口に出す
					x = (float)out.x;
					y = (float)out.y + ph;
				}

				return;
			}
		}
	}

	//--------------------------------------
	// クリア
	//--------------------------------------

/*	if (field->IsGoal(x + 32, y + 32)) {
		// プレイ中に計測している値を使う
		bool noMiss = (g_RetryCount == 0);

		ClearScene::SetResult(g_ClearTimeSeconds, g_RetryCount, noMiss);
		SceneManager::ChangeScene("CLEAR");
	}
*/
}

//--------------------------------------
// Draw()
//--------------------------------------
void Player::Draw()
{
	Field* field = FindGameObject<Field>();

	//--------------------------------------
	// 現在のスプライトシート上での位置を算出
	//--------------------------------------
	int xRect = (animIndex % ATLAS_WIDTH) * CHARACTER_WIDTH;
	int yRect = (animIndex / ATLAS_WIDTH) * CHARACTER_HEIGHT;

	//--------------------------------------
	// キャラクター描画
	// DrawRectGraph(x, y, 切り出しX, 切り出しY, 幅, 高さ, 画像, 透過, 左右反転)
	//--------------------------------------
	DrawRectGraph(static_cast<int>(x), static_cast<int>(y), xRect, yRect, CHARACTER_WIDTH, CHARACTER_HEIGHT, hImage, TRUE, frip);

	// デバッグ用：当たり判定円を描く（必要なときだけ）

	float cx, cy, r;
	GetHitCircle(cx, cy, r);
	int c = GetColor(0, 255, 0);
	DrawCircle((int)cx, (int)cy, (int)r, c, FALSE);


	//--------------------------------------
	// デバッグ用座標表示
	//--------------------------------------
	SetFontSize(30);
	int h = GetFontSize();

	//DrawFormatString(0, 200 + h * 0, GetColor(255, 255, 255), "PlayerX: %.2f", x);
	//DrawFormatString(0, 200 + h * 1, GetColor(255, 255, 255), "PlayerY: %.2f", y);
	//DrawFormatString(0, 200 + h * 2, GetColor(255, 255, 255), "PlayerHP: %d", hp);

}

void Player::ForceDie()
{
	x = -9999;
	y = -9999;

	// 動けなくする
	velocity = 0;
	onGround = false;

	// ゲームオーバーに飛ぶならここで～
	//SceneManager::ChangeScene("GAMEOVER");
}
