#pragma once
#include "../Library/SceneBase.h"
#include "../Library/GameObject.h"

class PlayScene : public SceneBase
{
public:
	PlayScene();
	~PlayScene();

	// ステージ番号（ステージセレクトから設定される）
	static int SelectedStage;

	// --- 残機・スコア・タイム ---
	static int   LifeCount;     // 残機
	static int   Score;         // 現在スコア
	static int   RetryCount;    // 死亡回数（リトライ回数）
	static float ElapsedTime;   // 経過時間（秒）

	// クリア時に保存しておく最終結果
	static int   FinalScore;
	static char  FinalRank;     // 'S','A','B','C','D'

	// プレイヤー死亡 / クリア時に呼ぶ用の関数
	static void  OnPlayerDeath();
	static void  OnStageClear();

	void Update() override;
	void Draw() override;

private:
	int  Ssound;
	bool sound;

	bool deathHandled;          // このプレイ中、死亡処理を一度だけ行うためのフラグ
};
