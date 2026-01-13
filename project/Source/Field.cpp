#include "Field.h"
#include "Collision.h"
#include "Player.h"
#include "CsvReader.h"
#include "Trap.h"
#include "Telop.h"
#include "SmallTrap.h"
#include "SmallTrap2.h"
#include "FallingFloor.h"
#include "FakeFloor.h"
#include "FlyingSpike.h"
#include "Dokan.h"
#include "Dokan2.h"
#include "VanishingFloor.h"
#include "RollingBall.h"
#include "FallingSpike.h"
#include "EnemyChaser.h"
#include "Boss.h"
#include "MovingWall.h"
#include "HiddenSpike.h"
#include "DirectionalSpike.h"
#include "LaserTurret.h"
#include "PatrolEnemy.h"
#include <DxLib.h>

// ブロック扱い（押し戻し/床/壁として固いセル）
static bool IsSolidCell(int cell)
{
	return (cell == 1 || cell == 7 || cell == 8);
}

//------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------
Field::Field(int stage)
{
	new Telop();

	char filename[60];
	sprintf_s<60>(filename, "data/stage%02d.csv", stage);

	// CSV 読み込み
	{
		CsvReader* csv = new CsvReader(filename);
		int lines = csv->GetLines();
		maps.resize(lines);

		for (int y = 0; y < lines; y++) {
			int cols = csv->GetColumns(y);
			maps[y].resize(cols);

			for (int x = 0; x < cols; x++) {
				maps[y][x] = csv->GetInt(y, x);
			}
		}
		delete csv;
	}

	// 画像
	SetDrawOrder(50);
	hImage = LoadGraph("data/image/NewBlock.png");
	fallingSpikeImage = LoadGraph("data/image/hariBottom.png");

	int w = 0;
	int h = 0;
	GetGraphSize(fallingSpikeImage, &w, &h);
	fallingSpikeWidth = w;
	fallingSpikeHeight = h;

	LoadDivGraph("data/image/Goal.png", GOAL_ANIM_FRAMES, GOAL_ANIM_FRAMES, 1, 64, 64, goalImages);

	x = 0;
	y = 0;
	scrollX = 0;

	// マップ走査して配置
	for (int yy = 0; yy < (int)maps.size(); yy++)
	{
		for (int xx = 0; xx < (int)maps[yy].size(); xx++)
		{
			int cell = maps[yy][xx];

			if (cell == 2)
			{
				new Player(xx * 64, yy * 64);
			}
			else if (cell == 3)
			{
				new Trap(xx * 64, yy * 64 + 64);
			}
			else if (cell == 4)
			{
				new FallingFloor(xx * 64, yy * 64);
			}
			else if (cell == 6)
			{
				new FakeFloor(xx * 64, yy * 64);
			}
			else if (cell == 7)
			{
				new Dokan(xx * 64, yy * 64);
				pipesIn.push_back({ xx * 64, yy * 64 });
			}
			else if (cell == 8)
			{
				new Dokan2(xx * 64, yy * 64);
				pipesOut.push_back({ xx * 64, yy * 64 });
			}
			else if (cell == 10)
			{
				new VanishingFloor(xx * 64, yy * 64);
			}
			else if (cell == 13)
			{
				// 落下針トリガー（1トリガー = 1針）
				FallingSpikeTrigger trig;
				trig.triggerPos = { xx * 64, yy * 64 };
				trig.activated = false;
				trig.timer = 0;
				trig.spikeIndex = -1;
				fallingSpikeTriggers.push_back(trig);
			}
			else if (cell == 14 || cell == 15)
			{
				// 上から落ちてくる針本体
				FallingSpikeInfo info;
				info.pos = { xx * 64, yy * 64 };
				info.alive = true;
				info.chaser = (cell == 15); // 15は追尾する針

				fallingSpikes.push_back(info);
			}
			else if (cell == 16)
			{
				// 近づいたら姿を見せる隠しトゲ
				new HiddenSpike(xx * 64.0f, yy * 64.0f, 90.0f);
			}
			else if (cell == 20)
			{
				ballTriggers.push_back({ xx * 64, yy * 64 });
				ballTriggered.push_back(false);
				ballTimer.push_back(0);
			}
			else if (cell == 21)
			{
				ballSpawns.push_back({ xx * 64, yy * 64 });
			}
			else if (cell == 30)
			{
				// 上向き針
				new DirectionalSpike(xx * 64.0f, yy * 64.0f, SpikeDir::Up);
			}
			else if (cell == 31)
			{
				// 下向き針
				new DirectionalSpike(xx * 64.0f, yy * 64.0f, SpikeDir::Down);
			}
			else if (cell == 32)
			{
				// 左向き針
				new DirectionalSpike(xx * 64.0f, yy * 64.0f, SpikeDir::Left);
			}
			else if (cell == 33)
			{
				// 右向き針
				new DirectionalSpike(xx * 64.0f, yy * 64.0f, SpikeDir::Right);
			}
			else if (cell == 40)
			{
				wallTriggers.push_back({ xx * 64, yy * 64 });
				wallTriggered.push_back(false);
			}
			else if (cell == 41)
			{
				wallSpawns.push_back({ xx * 64, yy * 64 });
			}
			else if (cell == 51)
			{
				// 右向きレーザー砲台
				new LaserTurret(xx * 64.0f, yy * 64.0f, LaserTurret::Dir::Right);
			}
			else if (cell == 52)
			{
				// 左向き
				new LaserTurret(xx * 64.0f, yy * 64.0f, LaserTurret::Dir::Left);
			}
			else if (cell == 53)
			{
				// 上向き
				new LaserTurret(xx * 64.0f, yy * 64.0f, LaserTurret::Dir::Up);
			}
			else if (cell == 54)
			{
				// 下向き
				new LaserTurret(xx * 64.0f, yy * 64.0f, LaserTurret::Dir::Down);
			}
			else if (cell == 60)
			{
				new PatrolEnemy(xx * 64, yy * 64);
				}
			else if (cell == 90)
			{
				new EnemyChaser(xx * 64, yy * 64);
			}
			else if (cell == 91)
			{
				new Boss(xx * 64, yy * 64 - 192);
			}
			else if (cell == 101)
			{
				new SmallTrap(xx * 64.0f + 24.0f, yy * 64.0f + 48.0f,SmallSpikeDir::Up);
			}
			else if (cell == 102)
			{
				new SmallTrap(xx * 64.0f + 24.0f, yy * 64.0f, SmallSpikeDir::Down);
			}
			else if (cell == 103)
			{
				new SmallTrap(xx * 64.0f + 24.0f, yy * 64.0f+24.0f, SmallSpikeDir::Left);
			}
			else if (cell == 104)
			{
				new SmallTrap(xx * 64.0f, yy * 64.0f+24.0f, SmallSpikeDir::Right);
			}
		}
	}

	// それぞれのトリガーに「真上にある針」を対応付ける
	for (auto& trig : fallingSpikeTriggers)
	{
		int trigX = trig.triggerPos.x;
		int trigY = trig.triggerPos.y;

		int bestIndex = -1;
		int bestDy = 100000000;

		// すべての落下針候補をチェック
		for (int i = 0; i < (int)fallingSpikes.size(); ++i)
		{
			auto& info = fallingSpikes[i];

			// 同じ列（X座標が同じ）の針だけを見る
			if (info.pos.x != trigX) continue;

			// トリガーより上にある針だけ対象
			if (info.pos.y >= trigY) continue;

			int dy = trigY - info.pos.y;  // 縦方向の距離（小さいほど近い）

			if (dy < bestDy)
			{
				bestDy = dy;
				bestIndex = i;
			}
		}

		// 見つからなかった場合は -1 のまま(なにも落ちない)
		trig.spikeIndex = bestIndex;
	}

}

//------------------------------------------------------------
// デストラクタ（A対策：画像解放）
//------------------------------------------------------------
Field::~Field()
{
	if (hImage != -1) DeleteGraph(hImage);
	if (fallingSpikeImage != -1) DeleteGraph(fallingSpikeImage);
	for (int i = 0;i < GOAL_ANIM_FRAMES;++i)
	{
		if (goalImages[i] != 1)
		{
			DeleteGraph(goalImages[i]);
		}
	}
}

//------------------------------------------------------------
// Update()
//------------------------------------------------------------
void Field::Update()
{
	Player* player = FindGameObject<Player>();
	if (!player) return;

	float px = player->GetX();
	float py = player->GetY();

	int tx = int(px + 32) / 64;
	int ty = int(py + 63) / 64;

	if (ty < 0 || ty >= (int)maps.size()) return;
	if (tx < 0 || tx >= (int)maps[ty].size()) return;

	goalAnimTimer++;
	if (goalAnimTimer >= goalAnimInterval)
	{
		goalAnimTimer = 0;
		goalAnimFrame++;
		if (goalAnimFrame >= GOAL_ANIM_FRAMES)
		{
			goalAnimFrame = 0;
		}
	}

	//------------------------------------------
	// 待機状態の落下針との当たり判定
	//------------------------------------------
	{
		VECTOR center = VGet(px + 32.0f, py + 32.0f, 0.0f);
		float  radius = player->GetRadius();  // Player に合わせて

		for (auto& s : fallingSpikes)
		{
			if (!s.alive) continue; // もう落下開始した針は無視

			float sx = (float)s.pos.x;
			float sy = (float)s.pos.y;

			VECTOR t1 = VGet(sx, sy, 0.0f);                     // 左上
			VECTOR t2 = VGet(sx + fallingSpikeWidth, sy, 0.0f);                     // 右上
			VECTOR t3 = VGet(sx + fallingSpikeWidth / 2.0f, sy + fallingSpikeHeight, 0.0f);// 下の先端

			if (HitCheck_Circle_Triangle(center, radius, t1, t2, t3))
			{
				player->ForceDie();
				player->SetDead();
				return; // 死んだら他の処理はスキップ
			}
		}
	}

	//------------------------------------------
	// 転がってくる球
	//------------------------------------------
	for (int i = 0; i < (int)ballTriggers.size(); i++)
	{
		if (i >= (int)ballTriggered.size() || i >= (int)ballTimer.size()) break;

		int trigX = ballTriggers[i].x / 64;
		int trigY = ballTriggers[i].y / 64;

		if (!ballTriggered[i] && tx == trigX && ty == trigY)
		{
			ballTriggered[i] = true;
			ballTimer[i] = 0;
		}
	}

	for (int i = 0; i < (int)ballTriggers.size(); i++)
	{
		if (i >= (int)ballTriggered.size() || i >= (int)ballTimer.size()) break;
		if (!ballTriggered[i]) continue;

		if (ballTimer[i] > 0)
		{
			ballTimer[i]--;
			continue;
		}

		if (i < (int)ballSpawns.size())
		{
			POINT spawn = ballSpawns[i];
			new RollingBall(spawn.x, spawn.y, -1.0f);
		}

		ballTimer[i] = 95;
	}

	//------------------------------------------------------
	// 上から落下してくる針（トリガーごと）
	//------------------------------------------------------
	for (auto& trig : fallingSpikeTriggers)
	{
		int tX = trig.triggerPos.x / 64;
		int tY = trig.triggerPos.y / 64;

		// まだ起動していなくて、プレイヤーがトリガーマスに乗ったら起動
		if (!trig.activated && tx == tX && ty == tY)
		{
			trig.activated = true;
			trig.timer = 20;  // 落ちるまでの待ちフレーム（好みで調整）
		}

		if (!trig.activated)
		{
			continue;
		}

		// タイマー待ち
		if (trig.timer > 0)
		{
			trig.timer--;
			continue;
		}

		// ここに来たら「落とすタイミング」
		if (trig.spikeIndex >= 0 && trig.spikeIndex < (int)fallingSpikes.size())
		{
			auto& info = fallingSpikes[trig.spikeIndex];

			if (info.alive)
			{
				// chaser が true のものは、FallingSpike 側で追尾モードになる
				new FallingSpike(info.pos.x, info.pos.y, info.chaser);
				info.alive = false;
			}
		}

		// 一度落としたら、このトリガーは用済み
		trig.activated = false;
	}

	//------------------------------------------------------
	// 動く壁トラップ
	//------------------------------------------------------
	for (int i = 0; i < (int)wallTriggers.size(); ++i)
	{
		if (i >= (int)wallTriggered.size()) break;
		if (wallTriggered[i]) continue;

		int trigX = wallTriggers[i].x / 64;
		int trigY = wallTriggers[i].y / 64;

		if (tx == trigX && ty == trigY)
		{
			wallTriggered[i] = true;

			if (i < (int)wallSpawns.size())
			{
				POINT sp = wallSpawns[i];
				int dir = (player->GetX() < sp.x) ? -1 : +1;
				new MovingWall((float)sp.x, (float)sp.y, dir);
			}
		}
	}

	//------------------------------------------
	// その他
	//------------------------------------------
	int cell = maps[ty][tx];
	if (cell == 5)
	{
		SpawnFlyingSpike(tx * 64, ty * 64, -1.0f);
		maps[ty][tx] = 0;
		return;
	}
}

//------------------------------------------------------------
// Draw()
//------------------------------------------------------------
void Field::Draw()
{
	// ブロック＆ゴール描画
	for (int yy = 0; yy < (int)maps.size(); yy++)
	{
		for (int xx = 0; xx < (int)maps[yy].size(); xx++)
		{
			int cell = maps[yy][xx];

			// 通常ブロック
			if (cell == 1)
			{
				DrawRectGraph(xx * 64, yy * 64, 0, 0, 64, 64, hImage, TRUE);
			}

			// ゴールマス（9 / 99）にアニメーション描画
			if (cell == 9 || cell == 99)
			{
				const float GOAL_SCALE = 2.1f;

				int gw, gh;
				GetGraphSize(goalImages[goalAnimFrame], &gw, &gh); // ここでは 64x64 のはず

				int drawW = int(gw * GOAL_SCALE);
				int drawH = int(gh * GOAL_SCALE);

				// マスの中心座標（1マスが 64x64 前提）
				int cx = xx * 64 + 32;
				int cy = yy * 64 + 32;

				// 中心合わせで拡大描画
				DrawExtendGraph(cx - drawW / 2, cy - drawH / 2, cx + drawW / 2, cy + drawH / 2, goalImages[goalAnimFrame], TRUE);
			}
		}
	}

	// 待機状態の針（13/14/15）
	for (auto& s : fallingSpikes)
	{
		if (!s.alive) continue;
		DrawGraph(s.pos.x, s.pos.y, fallingSpikeImage, TRUE);
	}
}


//------------------------------------------------------------
// 当たり判定（押し戻し）
//------------------------------------------------------------
int Field::HitCheckRight(int px, int py)
{
	if (py < 0) return 0;

	int hit = 0;

	// --- まず通常ブロック（マップ） ---
	int x = px / 64;
	int y = py / 64;

	if (y >= 0 && y < (int)maps.size() &&
		x >= 0 && x < (int)maps[y].size() &&
		IsSolidCell(maps[y][x]))
	{
		hit = px % 64 + 1;
	}

	// --- 移動壁も見る ---
	if (auto wall = FindGameObject<MovingWall>())
	{
		int w = wall->HitCheckRight(px, py);
		if (w > hit) hit = w;
	}

	return hit;
}

int Field::HitCheckLeft(int px, int py)
{
	if (py < 0) return 0;

	int hit = 0;

	// 通常ブロック
	int x = px / 64;
	int y = py / 64;

	if (y >= 0 && y < (int)maps.size() &&
		x >= 0 && x < (int)maps[y].size() &&
		IsSolidCell(maps[y][x]))
	{
		hit = 64 - (px % 64);
	}

	// 移動壁
	if (auto wall = FindGameObject<MovingWall>())
	{
		int w = wall->HitCheckLeft(px, py);
		if (w > hit) hit = w;
	}

	return hit;
}

int Field::HitCheckUp(int px, int py)
{
	if (py < 0) return 0;

	int hit = 0;

	// 通常ブロック
	int x = px / 64;
	int y = py / 64;

	if (y >= 0 && y < (int)maps.size() &&
		x >= 0 && x < (int)maps[y].size() &&
		IsSolidCell(maps[y][x]))
	{
		hit = 64 - (py % 64);
	}

	// 移動壁
	if (auto wall = FindGameObject<MovingWall>())
	{
		int w = wall->HitCheckUp(px, py);
		if (w > hit) hit = w;
	}

	return hit;
}


int Field::HitCheckDown(int px, int py)
{
	if (py < 0) return 0;

	int hit = 0;

	// 通常ブロック
	int x = px / 64;
	int y = py / 64;

	if (y >= 0 && y < (int)maps.size() &&
		x >= 0 && x < (int)maps[y].size() &&
		IsSolidCell(maps[y][x]))
	{
		hit = (py % 64) + 1;
	}

	// 移動壁
	if (auto wall = FindGameObject<MovingWall>())
	{
		int w = wall->HitCheckDown(px, py);
		if (w > hit) hit = w;
	}

	return hit;
}

bool Field::IsBlock(int tx, int ty)
{
	int cell = GetCell(tx, ty);
	return IsSolidCell(cell);
}

void Field::SpawnFlyingSpike(float x, float y, float direction)
{
	float speed = 30.0f * direction;
	new FlyingSpike(x + 64 * 5, y, speed);
}

bool Field::IsGoal(int px, int py)
{
	int tx = px / 64;
	int ty = py / 64;

	int cell = GetCell(tx, ty);
	return (cell == 9 || cell == 99);
}

int Field::GetCell(int tx, int ty)
{
	if (ty < 0 || ty >= (int)maps.size()) return -1;
	if (tx < 0 || tx >= (int)maps[ty].size()) return -1;
	return maps[ty][tx];
}