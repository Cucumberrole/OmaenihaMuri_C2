#include "Field.h"
#include "Player.h"
#include "CsvReader.h"
#include <vector>
using namespace std;

//------------------------------------------------------------
// マップデータ
//------------------------------------------------------------
// 1: ブロック
// 0: 空間
// 2: プレイヤー
//------------------------------------------------------------
vector<vector<int>> maps;
/*					{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
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

Field::Field(int stage)
{
	char filename[60];
	sprintf_s<60>(filename, "data/stage%02d.csv", stage);  // 例: data/stage01.csv

	// --- CSVからマップを読み込む ---
	CsvReader* csv = new CsvReader(filename);
	int lines = csv->GetLines();          // 行数（縦のマス数）
	maps.resize(lines);                   // マップ行数を設定

	for (int y = 0; y < lines; y++) {
		int cols = csv->GetColumns(y);    // 列数（横のマス数）
		maps[y].resize(cols);             // 行の列数を設定

		for (int x = 0; x < cols; x++) {
			int num = csv->GetInt(y, x);  // CSVの整数値を取得
			maps[y][x] = num;             // mapsに格納
		}
	}
	delete csv;

	// 背景画像とブロック画像を読み込む
	kabehImage = LoadGraph("data/image/kabe.png");
	hImage = LoadGraph("data/image/New Blo.png");

	x = 0;
	y = 0;
	scrollX = 0;

	// --- マップ走査してプレイヤー配置 ---
	for (int y = 0; y < maps.size(); y++)
	{
		for (int x = 0; x < maps[y].size(); x++)
		{
			if (maps[y][x] == 2)
			{
					new Player(x * 64, y * 64);
			}
		}
	}
}

Field::~Field() {}

void Field::Update()
{
	// 右に自動スクロールさせる場合
	// scrollX += 1;
}

void Update() {}

void Field::Draw()
{
	// 背景画像を描画
	DrawGraph(0, 0, kabehImage, TRUE);

	// マップ上の全ブロックを描画
	for (int y = 0; y < maps.size(); y++)
	{
		for (int x = 0; x < maps[y].size(); x++)
		{
			if (maps[y][x] == 1)
			{
				DrawRectGraph(x * 64, y * 64, 0, 0, 64, 64, hImage, 1);
			}
		}
	}
}

//------------------------------------------------------------
// 当たり判定処理（上下左右）
//------------------------------------------------------------

// 右方向の当たり判定
int Field::HitCheckRight(int px, int py)
{
	if (py < 0) {
		return 0;
	}
	int x = px / 64;
	int y = py / 64;
	if (maps[y][x] == 1)
	{
		// ブロック右端にめり込んだ分を押し戻す距離を返す
		return px % 64 + 1;
	}
	return 0;
}

// 左方向の当たり判定
int Field::HitCheckLeft(int px, int py)
{
	if (py < 0) {
		return 0;
	}
	int x = px / 64;
	int y = py / 64;
	if (maps[y][x] == 1)
	{
		return 64 - px % 64;
	}
	return 0;
}

// 下方向の当たり判定
int Field::HitCheckDown(int px, int py)
{
	if (py < 0) {
		return 0;
	}
	int x = px / 64;
	int y = py / 64;
	if (maps[y][x] == 1)
	{
		return py % 64 + 1;
	}
	return 0;
}

// 上方向の当たり判定
int Field::HitCheckUp(int px, int py)
{
	if (py < 0) {
		return 0;
	}
	int x = px / 64;
	int y = py / 64;
	if (maps[y][x] == 1)
		return 64 - py % 64;
	return 0;
}
