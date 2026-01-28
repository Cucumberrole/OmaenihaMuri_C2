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
	g_GameResult = GameResult{};        // すべてデフォルト値へ戻す
	g_GameResult.course = course;       // 難易度を設定

	g_GameResult.fixed = false;         // まだ確定していない
	g_GameResult.deathCount = 0;        // 死亡回数リセット

	g_GameResult.startTimeMs = GetNowCount(); // 計測開始時刻を保存
	g_GameResult.elapsedMs = 0;              // 経過時間リセット
}

// プレイ中に毎フレーム呼ぶ：経過時間を更新（確定後は更新しない）
void GR_UpdateDuringPlay()
{
	if (g_GameResult.fixed) return; // ゴール後は止める（重要）
	g_GameResult.elapsedMs = GetNowCount() - g_GameResult.startTimeMs;
}

// 死亡した瞬間に呼ぶ：死亡回数を増やす（確定後は増やさない）
void GR_OnDeath()
{
	if (g_GameResult.fixed) return;
	g_GameResult.deathCount++;
}

// 現在の経過時間を「秒」で返す（HUD表示用）
int GR_GetElapsedSecLive()
{
	const int ms = max(0, g_GameResult.elapsedMs); // 念のため負数ガード
	return ms / 1000;                              // ミリ秒→秒（切り捨て）
}

// ゴールした瞬間に呼ぶ：タイム/スコア/ランクを1回だけ確定する
void GR_FixOnGoalOnce()
{
	if (g_GameResult.fixed) return;  // 2回目以降は何もしない
	g_GameResult.fixed = true;

	g_GameResult.clearTimeSec = GR_GetElapsedSecLive();   // クリア秒を確定
	g_GameResult.score = CalcScore(g_GameResult);         // スコア確定
	g_GameResult.rank = CalcRank(g_GameResult);          // ランク確定
}

// ゴール時に「外部から渡された値」で確定したい場合（PlayScene側の値をそのまま渡す）
void GR_FixOnGoalOnce_Manual(int clearTimeSec, int deathCount)
{
	if (g_GameResult.fixed) return;  // 2回目以降は何もしない
	g_GameResult.fixed = true;

	g_GameResult.clearTimeSec = max(0, clearTimeSec); // 渡されたクリア秒を確定
	g_GameResult.deathCount = max(0, deathCount);   // 渡された死亡回数を確定

	g_GameResult.score = CalcScore(g_GameResult);     // スコア確定
	g_GameResult.rank = CalcRank(g_GameResult);      // ランク確定
}
