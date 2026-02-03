#include "GameResult.h"
#include <DxLib.h>
#include <algorithm>

GameResult g_GameResult;

// スコアがマイナスや異常に大きくならないように丸める
static int ClampScore(int s)
{
	s = max(0, s);         // 最小0点
	s = min(9999999, s);   // 最大9999999点（表示崩れ防止）
	return s;
}

// 難易度に応じて「死亡1回あたりの減点」を返す
static int DeathPenalty(const GameResult& gr)
{
	return (gr.course == CourseType::Easy) ? gr.easyDeathPenalty : gr.hardDeathPenalty;
}

// スコア計算
static int CalcScore(const GameResult& gr)
{
	const int elapsedSec = max(0, gr.clearTimeSec); // 念のため負数ガード

	int score = gr.initialScore;                    // 初期点
	score -= elapsedSec * gr.timePenaltyPerSec;     // 時間減点
	score -= gr.deathCount * DeathPenalty(gr);      // 死亡回数減点

	// 条件ボーナス
	if (gr.deathCount == 0) score += gr.noMissBonus; // ノーミス
	if (elapsedSec <= 60)   score += gr.under60sBonus; // 60秒以内

	return ClampScore(score);
}

// ランク判定
// Easy : S>=9600, A>=8800, B>=7500, C>=6500, else D
// Hard : S>=10000, A>=9000, B>=7800, C>=6500, else D
static char CalcRank(const GameResult& gr)
{
	const int s = gr.score; // 確定済みスコアに対して判定する

	if (gr.course == CourseType::Easy)
	{
		if (s >= 9600) return 'S';
		if (s >= 8800) return 'A';
		if (s >= 7500) return 'B';
		if (s >= 6500) return 'C';
		return 'D';
	}
	else
	{
		if (s >= 10000) return 'S';
		if (s >= 9000)  return 'A';
		if (s >= 7800)  return 'B';
		if (s >= 6500)  return 'C';
		return 'D';
	}
}

// プレイ開始時に呼ぶ：結果を初期化して計測開始
void GR_ResetRun(CourseType course)
{
	g_GameResult = GameResult{};
	g_GameResult.course = course;

	g_GameResult.fixed = false;
	g_GameResult.deathCount = 0;

	g_GameResult.startTimeMs = GetNowCount();
	g_GameResult.elapsedMs = 0;

	g_GameResult.liveScore = g_GameResult.initialScore;
	g_GameResult.appliedTimeSec = 0;
}

// プレイ中に毎フレーム呼ぶ：経過時間を更新（確定後は更新しない）
void GR_UpdateDuringPlay()
{
	if (g_GameResult.fixed) return;

	// 一時停止中は更新しない（タイマー停止）
	if (g_GameResult.paused) return;

	// 経過ms（pause分を差し引く）
	const int now = GetNowCount();
	g_GameResult.elapsedMs = (now - g_GameResult.startTimeMs) - g_GameResult.pausedTotalMs;
	if (g_GameResult.elapsedMs < 0) g_GameResult.elapsedMs = 0;

	// 時間減点（秒が進んだ分だけ適用）
	const int elapsedSec = g_GameResult.elapsedMs / 1000;
	if (elapsedSec > g_GameResult.appliedTimeSec)
	{
		const int deltaSec = elapsedSec - g_GameResult.appliedTimeSec;
		g_GameResult.liveScore -= deltaSec * g_GameResult.timePenaltyPerSec;
		g_GameResult.liveScore = ClampScore(g_GameResult.liveScore);

		g_GameResult.appliedTimeSec = elapsedSec;
	}
}


// 死亡した瞬間に呼ぶ：死亡回数を増やす（確定後は増やさない）
void GR_OnDeath()
{
	if (g_GameResult.fixed) return;

	g_GameResult.deathCount++;

	g_GameResult.liveScore -= DeathPenalty(g_GameResult);
	g_GameResult.liveScore = ClampScore(g_GameResult.liveScore);
}

// 現在の経過時間を「秒」で返す（HUD表示用）
int GR_GetElapsedSecLive()
{
	const int ms = max(0, g_GameResult.elapsedMs); // 念のため負数ガード
	return ms / 1000;                              // ミリ秒→秒（切り捨て）
}

int GR_GetLiveScore()
{
	return ClampScore(g_GameResult.liveScore);
}

// ゴールした瞬間に呼ぶ：タイム/スコア/ランクを1回だけ確定する
void GR_FixOnGoalOnce()
{
	if (g_GameResult.fixed) return;
	g_GameResult.fixed = true;

	g_GameResult.clearTimeSec = GR_GetElapsedSecLive();

	int finalScore = g_GameResult.liveScore;

	// ボーナス
	if (g_GameResult.deathCount == 0) finalScore += g_GameResult.noMissBonus;
	if (g_GameResult.clearTimeSec <= 60) finalScore += g_GameResult.under60sBonus;

	g_GameResult.score = ClampScore(finalScore);
	g_GameResult.rank = CalcRank(g_GameResult);
}

// ゴール時に「外部から渡された値」で確定したい場合（PlayScene側の値をそのまま渡す）
void GR_FixOnGoalOnce_Manual(int clearTimeSec, int deathCount)
{
	if (g_GameResult.fixed) return;
	g_GameResult.fixed = true;

	g_GameResult.clearTimeSec = max(0, clearTimeSec);
	g_GameResult.deathCount = max(0, deathCount);

	// Manualの場合も時間整合を取る（任意だが推奨）
	g_GameResult.elapsedMs = g_GameResult.clearTimeSec * 1000;
	g_GameResult.appliedTimeSec = g_GameResult.clearTimeSec;

	int finalScore = g_GameResult.liveScore;

	if (g_GameResult.deathCount == 0) finalScore += g_GameResult.noMissBonus;
	if (g_GameResult.clearTimeSec <= 60) finalScore += g_GameResult.under60sBonus;

	g_GameResult.score = ClampScore(finalScore);
	g_GameResult.rank = CalcRank(g_GameResult);
}

void GR_PauseTimer()
{
	if (g_GameResult.fixed) return;
	if (g_GameResult.paused) return;
	g_GameResult.paused = true;
	g_GameResult.pauseStartMs = GetNowCount();
}

void GR_ResumeTimer()
{
	if (g_GameResult.fixed) return;
	if (!g_GameResult.paused) return;
	const int now = GetNowCount();
	g_GameResult.pausedTotalMs += (now - g_GameResult.pauseStartMs);
	g_GameResult.paused = false;
}