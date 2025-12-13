#pragma once

struct GameResult
{
	int   score = 0;    // 最終スコア
	float clearTime = 0.0f; // クリアタイム(秒)
	int   retryCount = 0;    // リトライ回数
};

extern GameResult g_GameResult;

extern float g_ClearTimeSeconds;

extern int g_RetryCount;