#pragma once
#include <vector>
#include "../Library/GameObject.h"

class Field : public GameObject
{
public:
	Field(int stage);
	~Field();

	void Update() override;
	void Draw() override;

	// “固いタイルか？”（ブロック・土管など）
	bool IsBlock(int tx, int ty);

	void SpawnFlyingSpike(float x, float y, float direction);

	// ゴール判定（9 または 99 両対応にして安全側に）
	bool IsGoal(int px, int py);

	// マップのセル取得（範囲外は -1）
	int GetCell(int tx, int ty);

	// 土管
	std::vector<POINT> pipesIn;
	std::vector<POINT> pipesOut;

	// RollingBall 用（今は現状維持）
	std::vector<POINT> ballTriggers;
	std::vector<POINT> ballSpawns;
	std::vector<bool>  ballTriggered;
	std::vector<int>   ballTimer;

	//--------------------------------------------------------
	// 落下する針（struct で一括管理）
	//--------------------------------------------------------
	struct FallingSpikeInfo
	{
		POINT pos{};
		bool  alive = true;   // 天井で待機表示するか
		bool  chaser = false; // 落下後に追尾飛行するか（14用）
	};

	std::vector<FallingSpikeInfo> fallingSpikes;

	POINT fallingTrigger{ -9999, -9999 }; // 無効値で初期化
	bool  hasFallingTrigger = false;

	bool fallingActivated = false;
	int  fallingIndex = 0;
	int  fallingTimer = 0;

	//--------------------------------------------------------
	// 当たり判定（衝突チェック）
	//--------------------------------------------------------
	int HitCheckRight(int px, int py);
	int HitCheckLeft(int px, int py);
	int HitCheckDown(int px, int py);
	int HitCheckUp(int px, int py);

private:
	// マップはグローバルからメンバへ
	std::vector<std::vector<int>> maps;

	int hImage = -1;
	int fallingSpikeImage = -1;

	float x = 0;
	float y = 0;
	int scrollX = 0;

	// 動く壁トラップ
	std::vector<POINT> wallTriggers;
	std::vector<POINT> wallSpawns;
	std::vector<bool>  wallTriggered;
};