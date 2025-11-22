#include "Field.h"
#include "Player.h"
#include "CsvReader.h"
#include "Trap.h"
#include "FallingFloor.h"
#include "FakeFloor.h"
#include "FlyingSpike.h"
#include "Dokan.h"
#include "Dokan2.h"
#include <vector>
using namespace std;

//------------------------------------------------------------
// マップデータ
//------------------------------------------------------------
// 1 : ブロック（当たり判定あり）
// 0 : 空間（通過可）
// 2 : プレイヤー初期位置
// 3 : 針がにゅって出てくるトラップ
// 4 : 床落ちるトラップ
// 5 : 針が飛んでくるトラップ
// 6 : フェイクの床トラップ
// 8 : 土管
// 9 : ゴール
//------------------------------------------------------------
vector<vector<int>> maps;
/*					{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},　マップ元データ。とりま残しておきます
					{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
					{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
					{0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					{1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
*/
//------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------
Field::Field(int stage)
{
	char filename[60];
	sprintf_s<60>(filename, "data/stage%02d.csv", stage);

	//--------------------------------------------------------
	// --- CSVファイルからマップデータを読み込み ---
	//--------------------------------------------------------
	CsvReader* csv = new CsvReader(filename);
	int lines = csv->GetLines();      // 行数（縦方向のマス数）
	maps.resize(lines);               // 行数分のメモリを確保

	for (int y = 0; y < lines; y++) {
		int cols = csv->GetColumns(y);  // 列数（横方向のマス数）
		maps[y].resize(cols);           // 列数を設定

		for (int x = 0; x < cols; x++) {
			int num = csv->GetInt(y, x); // CSVの値を取得
			maps[y][x] = num;            // マップデータに格納
		}
	}
	delete csv;  // メモリ解放

	//--------------------------------------------------------
	// --- 背景画像とブロック画像を読み込み ---
	//--------------------------------------------------------
	SetDrawOrder(50);  // 描画順序を設定
	hImage = LoadGraph("data/image/New Blo.png");        // ブロック
	x = 0;
	y = 0;
	scrollX = 0;

	//--------------------------------------------------------
	// --- マップ走査してプレイヤー配置 ---
	//--------------------------------------------------------
	for (int y = 0; y < maps.size(); y++)
	{
		for (int x = 0; x < maps[y].size(); x++)
		{
			if (maps[y][x] == 2)
			{
				// CSVで「2」と指定された位置にプレイヤーを生成
				new Player(x * 64, y * 64);
			}

			if (maps[y][x] == 3)
			{
				// トラップ設置
				new Trap(x * 64, y * 64 + 64);
			}

			if (maps[y][x] == 4)
			{
				// 床落ちるトラップ
				new FallingFloor(x * 64, y * 64);
			}

			if (maps[y][x] == 6)
			{
				// フェイク床トラップ
				new FakeFloor(x * 64, y * 64);
			}

			if (maps[y][x] == 7)
			{
				// 土管入口
				new Dokan(x * 64, y * 64);
				POINT p = { x * 64, y * 64 };
				pipesIn.push_back(p);
			}
			if (maps[y][x] == 8)
			{
				// 土管出口
				new Dokan2(x * 64, y * 64);
				POINT p = { x * 64, y * 64 };
				pipesOut.push_back(p);
			}

		}
	}
}

//------------------------------------------------------------
// デストラクタ
//------------------------------------------------------------
Field::~Field() {}

//------------------------------------------------------------
// Update()
//------------------------------------------------------------
void Field::Update()
{
	Player* player = FindGameObject<Player>();
	if (player == nullptr) return;

	float px = player->GetX();
	float py = player->GetY();

	int tx = int(px + 32) / 64;  // プレイヤー中央
	int ty = int(py + 63) / 64;  // 足元

	if (maps[ty][tx] == 5)  // トリガーブロック
	{
		// --- 針を発射する ---
		SpawnFlyingSpike(tx * 64, ty * 64, -1.0f); // 左向き

		// １度踏んだら空白に書き換える
		maps[ty][tx] = 0;
	}
}

//------------------------------------------------------------
// Draw()
//------------------------------------------------------------
void Field::Draw()
{

	// --- マップ走査してブロック描画 ---
	for (int y = 0; y < maps.size(); y++)
	{
		for (int x = 0; x < maps[y].size(); x++)
		{
			if (maps[y][x] == 1)
			{
				// 各マスを64x64のタイルとして描画
				DrawRectGraph(
					x * 64,          // 描画X位置
					y * 64,          // 描画Y位置
					0, 0,            // 画像上の切り出し位置
					64, 64,          // タイルサイズ
					hImage,          // 画像ハンドル
					TRUE             // 透過あり
				);
			}
		}
	}
}

//------------------------------------------------------------
// 当たり判定処理（上下左右）
// ─────────────────────────────
// 引数 : px, py → プレイヤーの座標（左上基準）
// 戻り値 : 押し戻すべきピクセル数（0なら衝突なし）
//------------------------------------------------------------

// 右方向の当たり判定
int Field::HitCheckRight(int px, int py)
{
	if (py < 0) return 0;

	int x = px / 64;
	int y = py / 64;
	if (y >= maps.size()) return 0;

	if (maps[y][x] == 1) {
		// ブロック右端を超えた分だけ押し戻す
		return px % 64 + 1;
	}
	return 0;
}

// 左方向の当たり判定
int Field::HitCheckLeft(int px, int py)
{
	if (py < 0) return 0;

	int x = px / 64;
	int y = py / 64;
	if (y >= maps.size()) return 0;

	if (maps[y][x] == 1) {
		// ブロック左側にめり込んだ分を返す
		return 64 - (px % 64);
	}
	return 0;
}

// 上方向の当たり判定
int Field::HitCheckUp(int px, int py)
{
	if (py < 0) return 0;

	int x = px / 64;
	int y = py / 64;
	if (y >= maps.size()) return 0;

	if (maps[y][x] == 1) {
		// ブロックの下端にぶつかった分を返す
		return 64 - (py % 64);
	}
	return 0;
}

// 下方向の当たり判定
int Field::HitCheckDown(int px, int py)
{
	if (py < 0) return 0;

	int x = px / 64;
	int y = py / 64;
	if (y >= maps.size()) return 0;

	if (maps[y][x] == 1) {
		// 床ブロックにめり込んだ距離を返す
		return (py % 64) + 1;
	}
	return 0;
}

bool Field::IsBlock(int tx, int ty)
{
	if (ty < 0 || ty >= maps.size()) return false;
	if (tx < 0 || tx >= maps[ty].size()) return false;

	return maps[ty][tx] == 1;
}

void Field::SpawnFlyingSpike(float x, float y, float direction)
{
	float speed = 8.0f * direction; // 方向付きスピード

	// 針が飛んでくる
	new FlyingSpike(x + 64 * 4, y, speed);
}

bool Field::IsGoal(int px, int py)
{
	if (py < 0) {
		return 0;
	}
	int x = px / 64;
	int y = (py - 0) / 64;
	if (y >= maps.size())
		return 0;
	if (maps[y][x] == 9) {
		return true;
	}
	return false;
}
