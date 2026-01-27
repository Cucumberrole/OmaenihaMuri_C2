#pragma once

class Hud
{
public:
	// 1回だけ呼び出す
	static void Init();

	// ハートのアイコン
	static void SetHeartGraph(int heartGraphHandle);

	// HUDの描画
	// score: 現在のスコア
	// timeLeftSeconds: 残り時間（秒）
	// life: 現在のライフ数
	static void Draw(int score, int timeLeftSeconds, int life);

	static void Shutdown();
};