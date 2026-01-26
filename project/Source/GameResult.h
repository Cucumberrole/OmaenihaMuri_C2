#pragma once

// コース難易度（スコア計算の減点値・ランク閾値が変わる）
enum class CourseType
{
	Easy,
	Hard
};

// クリア時に表示する結果（タイム・スコア・ランク）をまとめたデータ
struct GameResult
{
	// ===== プレイ中に更新する値（ゴール確定前） =====

	CourseType course = CourseType::Easy; // 現在のコース難易度（Easy/Hard）

	int startTimeMs = 0;  // 計測開始時刻（GetNowCount()）
	int elapsedMs = 0;  // 経過時間（ミリ秒）※プレイ中に更新

	int deathCount = 0;  // 死亡回数（= リトライ回数として使う想定）

	// ===== ゴール時に確定する値（以降は変えない） =====

	bool fixed = false;  // 結果を確定したか？（true以降は値を更新しない）

	int  clearTimeSec = 0; // クリアタイム（秒）※ゴール時に確定
	int  score = 0; // 最終スコア        ※ゴール時に確定
	char rank = 'D'; // 最終ランク（S/A/B/C/D）※ゴール時に確定

	// ===== スコア計算の設定（好みで調整） =====

	int initialScore = 10000; // 初期スコア（ここから減点/加点する）
	int timePenaltyPerSec = 10;    // 1秒あたりの減点（例：-10点/秒）
	int easyDeathPenalty = 300;   // Easyの死亡（リトライ）1回あたりの減点
	int hardDeathPenalty = 600;   // Hardの死亡（リトライ）1回あたりの減点

	int noMissBonus = 2000;  // ノーミス（deathCount==0）ボーナス
	int under60sBonus = 1000;  // 60秒以内クリアボーナス
};

// どのシーンからでも参照できる結果データ（グローバル）
extern GameResult g_GameResult;

// ===== API（シーン側から呼ぶ関数）=====

// 新しくプレイを開始する時に呼ぶ
void GR_ResetRun(CourseType course);

// プレイ中に毎フレーム呼ぶ
void GR_UpdateDuringPlay();

// プレイヤーが死んだ瞬間に呼ぶ
void GR_OnDeath();

// HUD表示用：現在の経過秒を返す
int  GR_GetElapsedSecLive();

// ゴールした瞬間に呼ぶ：タイム/スコア/ランクを「1回だけ」確定する（複数回呼んでもOK）
void GR_FixOnGoalOnce();

void GR_FixOnGoalOnce_Manual(int clearTimeSec, int deathCount);
