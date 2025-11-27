#pragma once
#include <iostream>
#include <vector>
#include "../Library/GameObject.h"

class Field : public GameObject
{
public:
	Field(int stage);
	~Field();

	void Update() override;
	void Draw() override;

	bool IsBlock(int tx, int ty);
	void SpawnFlyingSpike(float x, float y, float direction);
	bool IsGoal(int px, int py);

	std::vector<POINT> pipesIn;   // 土管入り口(7)
	std::vector<POINT> pipesOut;  // 土管出口(8)

	void RemoveFloorsAround(int tx, int ty); // 床を消します

	//--------------------------------------------------------
	// 当たり判定（衝突チェック）
	//--------------------------------------------------------
	int HitCheckRight(int px, int py);  // 右方向の当たり判定
	int HitCheckLeft(int px, int py);   // 左方向の当たり判定
	int HitCheckDown(int px, int py);   // 下方向の当たり判定（床）
	int HitCheckUp(int px, int py);     // 上方向の当たり判定（天井）

private:
	int hImage;     // 地形（ブロック）画像ハンドル
	float x, y;     // ステージの座標（主にスクロール時に使用）
	int scrollX;    // 横スクロール量（カメラ位置）
};