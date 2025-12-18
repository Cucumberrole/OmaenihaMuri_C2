#pragma once
#include "../Library/GameObject.h"

class VanishingFloor : public GameObject
{
public:
	VanishingFloor(int sx, int sy);
	~VanishingFloor();

	void Update() override;
	void Draw() override;

	// 床として有効なときだけ当たり判定を返す
	bool IsActive() const { return isActive; }

	int HitCheckDown(int px, int py);
	int HitCheckUp(int px, int py);
	int HitCheckLeft(int px, int py);
	int HitCheckRight(int px, int py);

	static void ResetAll();

private:
	int floorImage;      // ブロック画像
	int spikeImage;      // 針画像
	float x, y;          // 左上座標

	bool isActive;       // true = 床として存在（ブロック画像＆足場あり）　false = 床は消えて針だけ（足場なし・ダメージあり）

	float vanishRange;   // プレイヤーが近づく距離

	// 全インスタンス共通のフラグ
	// どれか1つがトリガーされると、全て「床が消滅状態」になる
	static bool s_triggered;
};