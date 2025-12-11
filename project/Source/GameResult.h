#pragma once

struct GameResult
{
	int   score = 0;    // 最終スコア
	float clearTime = 0.0f; // クリアタイム(秒)
	int   retryCount = 0;    // リトライ回数
};

// どこからでも触れるように1個だけ用意
extern GameResult g_GameResult;
