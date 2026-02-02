#pragma once
#include "../Library/GameObject.h"
#include <vector>

class FallingSpike : public GameObject
{
public:
	FallingSpike(int sx, int sy, bool chaseAfterLand = false, int triggerGroupId = 0);
	~FallingSpike();

	void Update() override;
	void Draw() override;

	int fallingSpikeWidth = 64;
	int fallingSpikeHeight = 64;

	// トリガーから呼ぶ用
	void Activate();                     // この針だけ有効化
	int  GetTriggerGroupId() const { return triggerGroupId; }
	bool IsActive() const { return active; }

	// 指定グループの針を全部起動（トリガー側から使う）
	static void ActivateGroup(int groupId);

private:
	// すべての FallingSpike を登録しておく
	static std::vector<FallingSpike*> s_allSpikes;

	int   hImage;
	float x, y;
	float vy;
	float gravity;
	int   width;      // 画像幅
	int   height;     // 画像高さ

	bool  landed;     // 地面に着地したか

	// 追尾関連
	bool  isChaser;      // この針が「追尾針」かどうか
	bool  startedChase;  // 一度でも横移動を開始したか
	float vx;            // 横方向速度
	float chaseSpeed;    // 追尾時の基本スピード

	// トリガー関連
	int   triggerGroupId;   // どのトリガーグループに属するか
	bool  active;           // 今この針は動いて良いか

	const char* tauntText; // 煽り文字
	bool showTaunt;        // 表示フラグ

	int SpikeSE;
};
