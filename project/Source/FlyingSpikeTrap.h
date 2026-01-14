#pragma once
#include "../Library/GameObject.h"
#include <DxLib.h>
#include <vector>
#include <cmath>

#include "Player.h"
#include "Field.h"
#include "Collision.h" // HitCheck_Circle_Triangle を使う

// ※ SpikeDir と衝突しない名前
enum class TrapDir
{
	Left,
	Right,
	Up,
	Down
};

// トリガー + 発射 + 針の飛翔 + 当たり判定 + 描画 を全部まとめたクラス
// 仕様：プレイヤーが範囲に「入った瞬間」1回だけ発射。以後は二度と発射しない。
class FlyingSpikeTrap : public GameObject
{
public:
	// rect：トリガー範囲（ワールド座標の矩形）
	// fromDir：どこ側（画面外）から出現するか（Rightなら右画面外→左へ飛ぶ）
	// speed：速度（このトラップ固有）
	// laneWorld：NANならプレイヤー位置に合わせる。値ありならレーン固定（左右発射ならY固定、上下発射ならX固定）
	// spawnMargin：画面外スポーンの余白
	FlyingSpikeTrap(
		float rectX, float rectY, float rectW, float rectH,
		TrapDir fromDir,
		float speed,
		float laneWorld = NAN,
		float spawnMargin = 64.0f
	);

	~FlyingSpikeTrap();

	void Update() override;
	void Draw() override;

private:
	struct SpikeShot
	{
		float x, y;      // 左上
		float vx, vy;    // 速度
		TrapDir dir;     // 進行方向
		float speed;     // 速度の大きさ
		bool alive;
	};

private:
	// --- トリガー ---
	bool IsPlayerInside(Player* player) const;

	// --- 発射 ---
	void Fire(Player* player);
	void CalcSpawnParams(
		const VECTOR& targetCenter,
		float spikeW, float spikeH,
		float& outSpawnX, float& outSpawnY,
		TrapDir& outFlyDir
	) const;

	// --- 弾（針）更新 ---
	void UpdateShots(Player* player, Field* field);
	void BuildTriangleVertices(const SpikeShot& s, VECTOR& t1, VECTOR& t2, VECTOR& t3) const;

	// --- 画像共有 ---
	void EnsureSharedImageLoaded();
	static int sImage;
	static int sRefCount;

private:
	// トリガー範囲
	float x, y, w, h;

	// 発射設定
	TrapDir fromDir;
	float speed;
	float laneWorld;
	float spawnMargin;

	// 内部状態（1回発動のみ）
	bool wasInside;
	bool triggered;

	// 弾管理
	std::vector<SpikeShot> shots;

	// 画像サイズ（hariLeft.png を回転して描画）
	float spikeW;
	float spikeH;
};
