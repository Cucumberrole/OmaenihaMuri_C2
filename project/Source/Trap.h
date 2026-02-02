#pragma once
#include "../Library/GameObject.h"

class Trap : public GameObject
{
public:
	Trap(int x, int y);
	~Trap();
	void Update() override;
	void Draw() override;

	// --- 衝突判定用（使っていなければ消してもOK） ---
	bool CheckHit(int px, int py, int pw, int ph); // プレイヤー矩形と当たっているか？

private:
	int   hImage;    // 針の画像ハンドル
	float x, y;      // 座標（針が生える床の位置）
	int   width;     // 幅（画像サイズ）
	int   height;    // 高さ

	// --- 動き制御用 ---
	float moveSpeed;   // 上昇スピード
	float offsetY;     // にょきっと出る量（0〜height）
	bool  isActive;    // 発動中かどうか（上昇アニメ中）
	bool  isExtended;  // 出きった状態（出し切った後）

	void Activate();   // プレイヤーが踏んだときに発動

};