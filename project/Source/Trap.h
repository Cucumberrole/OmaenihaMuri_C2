#pragma once
#include "../Library/GameObject.h"

class Trap : public GameObject
{
public:
	Trap(int x, int y);
	~Trap();
	void Update() override;
	void Draw() override;

	// --- 衝突判定用 ---
	bool CheckHit(int px, int py, int pw, int ph); // プレイヤー矩形と当たっているか？

private:
	int hImage;    // 針の画像ハンドル
	float x, y;    // 座標
	int width;     // 幅（画像サイズ）
	int height;    // 高さ

	// --- 動き制御用 ---
	float offsetY;     // にょきっと出る量（0〜64）
	bool isActive;     // 発動中かどうか
	bool isExtended;   // 出きった状態
	float moveSpeed;   // 上昇スピード

	void Activate();   // プレイヤーが踏んだときに発動
};