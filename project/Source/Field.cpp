#include "Field.h"
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

	x = 0;
	y = 0;
	scrollX = 0;

	// 落下針のトリガー初期化（A対策）
	hasFallingTrigger = false;
	fallingTrigger = { -9999, -9999 };
	fallingActivated = false;
	fallingIndex = 0;
	fallingTimer = 0;

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
			else if (cell == 11)
			{
				new SmallTrap(xx * 64 + 24, yy * 64 + 48);
			}
			else if (cell == 12)
			{
				new SmallTrap2(xx * 64 + 24, yy * 64);
			}
			else if (cell == 13)
			{
				fallingTrigger = { xx * 64, yy * 64 };
				hasFallingTrigger = true;
			}
			else if (cell == 14)
			{
				FallingSpikeInfo info;
				info.pos = { xx * 64, yy * 64 };
				info.alive = true;
				info.chaser = false;
				fallingSpikes.push_back(info);
			}
			else if (cell == 15)
			{
				FallingSpikeInfo info;
				info.pos = { xx * 64, yy * 64 };
				info.alive = true;
				info.chaser = true;
				fallingSpikes.push_back(info);
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
			else if (cell == 40)
			{
				wallTriggers.push_back({ xx * 64, yy * 64 });
				wallTriggered.push_back(false);
			}
			else if (cell == 41)
			{
				wallSpawns.push_back({ xx * 64, yy * 64 });
			}
			else if (cell == 90)
			{
				new EnemyChaser(xx * 64, yy * 64);
			}
			else if (cell == 91)
			{
				new Boss(xx * 64, yy * 64 - 192);
			}
		}
	}
}

//------------------------------------------------------------
// デストラクタ（A対策：画像解放）
//------------------------------------------------------------
Field::~Field()
{
	if (hImage != -1) DeleteGraph(hImage);
	if (fallingSpikeImage != -1) DeleteGraph(fallingSpikeImage);
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
	// 上から落下してくる針
	//------------------------------------------------------
	if (hasFallingTrigger)
	{
		int tX = fallingTrigger.x / 64;
		int tY = fallingTrigger.y / 64;

		if (!fallingActivated && tx == tX && ty == tY)
		{
			fallingActivated = true;
			fallingIndex = 0;
			fallingTimer = 0;
		}

		if (fallingActivated)
		{
			if (fallingTimer > 0)
			{
				fallingTimer--;
			}
			else
			{
				if (fallingIndex < (int)fallingSpikes.size())
				{
					auto& info = fallingSpikes[fallingIndex];

					if (info.alive)
					{
						new FallingSpike(info.pos.x, info.pos.y, info.chaser);
						info.alive = false;
					}

					fallingIndex++;
					fallingTimer = 20;
				}
			}
		}
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
	// ブロック描画（1だけ）
	for (int yy = 0; yy < (int)maps.size(); yy++)
	{
		for (int xx = 0; xx < (int)maps[yy].size(); xx++)
		{
			if (maps[yy][xx] == 1)
			{
				DrawRectGraph(xx * 64, yy * 64, 0, 0, 64, 64, hImage, TRUE);
			}
		}
	}

	// 待機状態の針（13/14）
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

	int x = px / 64;
	int y = py / 64;

	if (y < 0 || y >= (int)maps.size()) return 0;
	if (x < 0 || x >= (int)maps[y].size()) return 0;

	if (IsSolidCell(maps[y][x])) return px % 64 + 1;
	return 0;
}

int Field::HitCheckLeft(int px, int py)
{
	if (py < 0) return 0;

	int x = px / 64;
	int y = py / 64;

	if (y < 0 || y >= (int)maps.size()) return 0;
	if (x < 0 || x >= (int)maps[y].size()) return 0;

	if (IsSolidCell(maps[y][x])) return 64 - (px % 64);
	return 0;
}

int Field::HitCheckUp(int px, int py)
{
	if (py < 0) return 0;

	int x = px / 64;
	int y = py / 64;

	if (y < 0 || y >= (int)maps.size()) return 0;
	if (x < 0 || x >= (int)maps[y].size()) return 0;

	if (IsSolidCell(maps[y][x])) return 64 - (py % 64);
	return 0;
}

int Field::HitCheckDown(int px, int py)
{
	if (py < 0) return 0;

	int x = px / 64;
	int y = py / 64;

	if (y < 0 || y >= (int)maps.size()) return 0;
	if (x < 0 || x >= (int)maps[y].size()) return 0;

	if (IsSolidCell(maps[y][x])) return (py % 64) + 1;
	return 0;
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