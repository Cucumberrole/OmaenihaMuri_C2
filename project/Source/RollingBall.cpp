#include "Field.h"
#include "Player.h"
#include "RollingBall.h"
#include "SoundCache.h"
#include <DxLib.h>

RollingBall::RollingBall(float sx, float sy, float dir)
{
	hImage = LoadGraph("data/image/ironball.png");

	x = sx;
	y = sy;

	vx = 5.0f * dir;   // dir = +1 で右、-1 で左
	vy = 0.0f;
	gravity = 0.4f;

	// 表示上のサイズ
	size = 64;

	int w, h;
	GetGraphSize(hImage, &w, &h);
	// 正方形前提　w が 0 にならないよう一応チェック
	if (w > 0)
	{
		scale = static_cast<float>(size) / static_cast<float>(w);
	}
	else
	{
		scale = 1.0f;
	}

	// 回転角初期化
	angle = 0.0f;

	BallSE = SoundCache::Get("data/BGM/death_crushed.mp3");
}

RollingBall::~RollingBall()
{
	DeleteGraph(hImage);
	StopSoundMem(BallSE);
}

void RollingBall::Update()
{
	Field* field = FindGameObject<Field>();
	if (!field) return;

	//----------------------------------------
	// 重力
	//----------------------------------------
	vy += gravity;
	y += vy;

	//----------------------------------------
	// 床との当たり判定（穴なら落下し続ける）
	//----------------------------------------
	int tx = static_cast<int>((x + size / 2) / 64);
	int ty = static_cast<int>((y + size) / 64);

	if (!field->IsBallBlock(tx, ty))
	{
		// 落下継続
	}
	else
	{
		// 床に着地
		y = ty * 64 - size;
		vy = 0;
	}

	//----------------------------------------
	// 横移動
	//----------------------------------------
	x += vx;

	//----------------------------------------
	// 壁にぶつかったら反転
	//----------------------------------------
	int cx = static_cast<int>((x + (vx > 0 ? size : 0)) / 64);
	int cy = static_cast<int>((y + size / 2) / 64);

	if (field->IsBlock(cx, cy))
	{
		Bounce();
	}

	//----------------------------------------
	// プレイヤーとの衝突判定（円×円）
	//----------------------------------------
	Player* player = FindGameObject<Player>();
	if (player)
	{
		// プレイヤーの情報
		float px = player->GetX();
		float py = player->GetY();
		float pw = 64.0f;
		float ph = 64.0f;

		// プレイヤーの円
		float playerCx = px + pw / 2.0f;
		float playerCy = py + ph / 2.0f;
		float playerR = pw * 0.4f;

		// ボールの円（size は表示上の直径 64 を想定）
		float ballCx = x + size / 2.0f;
		float ballCy = y + size / 2.0f;
		float ballR = size * 0.5f;

		// 中心距離の2乗と半径の和の2乗で判定
		float dx = ballCx - playerCx;
		float dy = ballCy - playerCy;
		float dist2 = dx * dx + dy * dy;
		float rSum = ballR + playerR;

		if (dist2 <= rSum * rSum)
		{
			PlaySoundMem(BallSE, DX_PLAYTYPE_BACK);
			player->ForceDie();
			player->SetDead();
		}
	}

	//----------------------------------------
	// 回転角の更新
	// vx の符号で回転方向が決まる
	//----------------------------------------
	// 係数 0.05f は「転がる速さに対してどれくらい回転するか」の調整用
	angle += vx * 0.05f;

	// 角度が大きくなりすぎないように 0～2π の範囲にしておく
	const float PI2 = 3.1415926535f * 2.0f;
	if (angle > PI2) angle -= PI2;
	if (angle < -PI2) angle += PI2;

	if (x <= 64 && y >= 960)
	{
		DestroyMe();
	}
}

void RollingBall::Bounce()
{
	// 進行方向を反転
	vx = -vx;
	// 回転方向は vx に依存しているので、
	// 次のフレーム以降、自動で逆回転になります。
}

void RollingBall::Draw()
{
	// x, y は左上なので、中心座標を計算する
	float cx = x + size / 2.0f;
	float cy = y + size / 2.0f;

	// 回転＆縮小して描画
	DrawRotaGraph(
		static_cast<int>(cx),
		static_cast<int>(cy),
		scale,
		angle,
		hImage,
		TRUE
	);
}