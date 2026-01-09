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
// cell番号 -> 生成処理 登録
//------------------------------------------------------------
void Field::RegisterCellSpawners()
{
	cellSpawners.clear();
	cellSpawners.reserve(64);

	// 2: Player
	cellSpawners.emplace(2, [this](int tx, int ty)
		{
			new Player(tx * 64, ty * 64);
		});

	// 3: PopUp Trap
	cellSpawners.emplace(3, [this](int tx, int ty)
		{
			new Trap(tx * 64, ty * 64 + 64);
		});

	cellSpawners.emplace(4, [this](int tx, int ty) { new FallingFloor(tx * 64, ty * 64); });
	cellSpawners.emplace(6, [this](int tx, int ty) { new FakeFloor(tx * 64, ty * 64); });
	cellSpawners.emplace(7, [this](int tx, int ty) { new Dokan(tx * 64, ty * 64); });
	cellSpawners.emplace(8, [this](int tx, int ty) { new Dokan2(tx * 64, ty * 64); });
	cellSpawners.emplace(10, [this](int tx, int ty) { new VanishingFloor(tx * 64, ty * 64); });
	cellSpawners.emplace(11, [this](int tx, int ty) { new SmallTrap(tx * 64 + 24, ty * 64 + 48); });
	cellSpawners.emplace(12, [this](int tx, int ty) { new SmallTrap2(tx * 64 + 24, ty * 64); });

	// 13: Falling spike trigger
	cellSpawners.emplace(13, [this](int tx, int ty)
		{
			FallingSpikeTrigger trig;
			trig.triggerPos = { tx * 64, ty * 64 };
			trig.activated = false;
			trig.timer = 0;
			trig.spikeIndex = -1;
			fallingSpikeTriggers.push_back(trig);
		});

	// 14/15: Falling spike body (15 is chaser)
	cellSpawners.emplace(14, [this](int tx, int ty)
		{
			FallingSpikeInfo info;
			info.pos = { tx * 64, ty * 64 };
			info.alive = true;
			info.chaser = false;
			fallingSpikes.push_back(info);
		});

	cellSpawners.emplace(15, [this](int tx, int ty)
		{
			FallingSpikeInfo info;
			info.pos = { tx * 64, ty * 64 };
			info.alive = true;
			info.chaser = true;
			fallingSpikes.push_back(info);
		});

	cellSpawners.emplace(16, [this](int tx, int ty)
		{
			new HiddenSpike(tx * 64.0f, ty * 64.0f, 90.0f);
		});

	// 20/21: Rolling ball trigger/spawn
	cellSpawners.emplace(20, [this](int tx, int ty)
		{
			ballTriggers.push_back({ tx * 64, ty * 64 });
			ballTriggered.push_back(false);
			ballTimer.push_back(0);
		});

	cellSpawners.emplace(21, [this](int tx, int ty)
		{
			ballSpawns.push_back({ tx * 64, ty * 64 });
		});

	// 30-33: Directional spikes
	cellSpawners.emplace(30, [this](int tx, int ty) { new DirectionalSpike(tx * 64.0f, ty * 64.0f, SpikeDir::Up); });
	cellSpawners.emplace(31, [this](int tx, int ty) { new DirectionalSpike(tx * 64.0f, ty * 64.0f, SpikeDir::Down); });
	cellSpawners.emplace(32, [this](int tx, int ty) { new DirectionalSpike(tx * 64.0f, ty * 64.0f, SpikeDir::Left); });
	cellSpawners.emplace(33, [this](int tx, int ty) { new DirectionalSpike(tx * 64.0f, ty * 64.0f, SpikeDir::Right); });

	// 40/41: Moving wall trigger/spawn
	cellSpawners.emplace(40, [this](int tx, int ty)
		{
			wallTriggers.push_back({ tx * 64, ty * 64 });
			wallTriggered.push_back(false);
		});

	cellSpawners.emplace(41, [this](int tx, int ty)
		{
			wallSpawns.push_back({ tx * 64, ty * 64 });
		});

	// 51-54: Laser turrets
	cellSpawners.emplace(51, [this](int tx, int ty) { new LaserTurret(tx * 64.0f, ty * 64.0f, LaserTurret::Dir::Right); });
	cellSpawners.emplace(52, [this](int tx, int ty) { new LaserTurret(tx * 64.0f, ty * 64.0f, LaserTurret::Dir::Left); });
	cellSpawners.emplace(53, [this](int tx, int ty) { new LaserTurret(tx * 64.0f, ty * 64.0f, LaserTurret::Dir::Up); });
	cellSpawners.emplace(54, [this](int tx, int ty) { new LaserTurret(tx * 64.0f, ty * 64.0f, LaserTurret::Dir::Down); });

	// Enemies / Boss
	cellSpawners.emplace(60, [this](int tx, int ty) { new PatrolEnemy(tx * 64, ty * 64); });
	cellSpawners.emplace(90, [this](int tx, int ty) { new EnemyChaser(tx * 64, ty * 64); });
	cellSpawners.emplace(91, [this](int tx, int ty) { new Boss(tx * 64, ty * 64 - 192); });
}

//------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------
Field::Field(int stage)
{
	new Telop();

	char filename[60];
	sprintf_s<60>(filename, "data/stage%02d.csv", stage);

	// CSV読み込み
	CsvReader reader(filename);

	maps.clear();

	const int lines = reader.GetLines();
	maps.resize(lines);

	for (int y = 0; y < lines; ++y)
	{
		const int cols = reader.GetColumns(y);
		maps[y].resize(cols);

		for (int x = 0; x < cols; ++x)
		{
			maps[y][x] = reader.GetInt(y, x);
		}
	}


	// マップ画像
	hImage = LoadGraph("data/image/field.png");

	// 落下針画像
	fallingSpikeImage = LoadGraph("data/image/hari.png");
	GetGraphSize(fallingSpikeImage, &fallingSpikeWidth, &fallingSpikeHeight);

	// ゴールアニメ（7枚）
	LoadDivGraph("data/image/Goal.png", GOAL_ANIM_FRAMES, GOAL_ANIM_FRAMES, 1, 64, 64, goalImages);

	x = 0;
	y = 0;
	scrollX = 0;

	RegisterCellSpawners();

	// マップ走査して配置
	for (int yy = 0; yy < (int)maps.size(); yy++)
	{
		for (int xx = 0; xx < (int)maps[yy].size(); xx++)
		{
			int cell = maps[yy][xx];

			auto it = cellSpawners.find(cell);
			if (it != cellSpawners.end())
			{
				it->second(xx, yy);
			}
		}
	}

	// それぞれのトリガーに「真上にある針」を対応付ける
	for (auto& trig : fallingSpikeTriggers)
	{
		int bestIndex = -1;
		int bestDy = 999999;

		for (int i = 0; i < (int)fallingSpikes.size(); i++)
		{
			auto& s = fallingSpikes[i];

			if (s.pos.x == trig.triggerPos.x && s.pos.y < trig.triggerPos.y)
			{
				int dy = trig.triggerPos.y - s.pos.y;
				if (dy < bestDy)
				{
					bestDy = dy;
					bestIndex = i;
				}
			}
		}

		trig.spikeIndex = bestIndex;
	}
}

Field::~Field()
{
	if (hImage != -1) DeleteGraph(hImage);
	if (fallingSpikeImage != -1) DeleteGraph(fallingSpikeImage);

	for (int i = 0; i < GOAL_ANIM_FRAMES; i++)
	{
		if (goalImages[i] != -1) DeleteGraph(goalImages[i]);
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