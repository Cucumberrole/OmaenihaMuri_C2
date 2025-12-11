#include "PlayScene.h"
#include "Field.h"
#include "Background.h"
#include "Player.h"
#include "../Library/SceneManager.h"
#include "../Library/Trigger.h"
#include "../Library/Time.h"

// ==== static 変数の定義 ====

// ステージ番号
int PlayScene::SelectedStage = -1;

// 残機・スコア・タイム
int   PlayScene::LifeCount = 0;
int   PlayScene::Score = 0;
int   PlayScene::RetryCount = 0;
float PlayScene::ElapsedTime = 0.0f;

// クリア結果
int   PlayScene::FinalScore = 0;
char  PlayScene::FinalRank = 'D';

// スコアの「経過時間による減点」制御用（1秒ごとに-10点するため）
static float s_timePenaltyAccum = 0.0f;


// コンストラクタ（ステージ開始）
PlayScene::PlayScene()
{
	// 背景とフィールド生成
	new Background();
	new Field(SelectedStage);

	sound = false;
	Ssound = LoadSoundMem("data/sound/bgm_ogg.ogg");

	// このプレイ（1回分）用の初期化
	deathHandled = false;
	ElapsedTime = 0.0f;
	s_timePenaltyAccum = 0.0f;
}

// デストラクタ
PlayScene::~PlayScene()
{
	InitSoundMem();
}

// プレイヤー死亡時に呼ばれる（減点＆リトライカウント）
void PlayScene::OnPlayerDeath()
{
	RetryCount++;          // リトライ回数
	Score -= 500;          // 死亡ペナルティ
	if (Score < 0) Score = 0;
}

// クリア時に呼ばれる（最終スコア＆ランク計算）
void PlayScene::OnStageClear()
{
	int final = Score;

	// ボーナス条件
	bool noMiss = (RetryCount == 0);
	bool timeBonus = (ElapsedTime <= 60.0f);

	if (noMiss)    final += 2000;  // ノーミス
	if (timeBonus) final += 1000;  // 1分以内クリア

	FinalScore = final;

	// ランク判定
	if (final >= 9600) FinalRank = 'S';
	else if (final >= 8500) FinalRank = 'A';
	else if (final >= 7000) FinalRank = 'B';
	else if (final >= 5000) FinalRank = 'C';
	else                    FinalRank = 'D';
}

// Update
void PlayScene::Update()
{
	// ==== タイムと時間経過による減点 ====
	// プレイヤーがまだ生きているときだけ進めたい場合は deathHandled を見る
	if (!deathHandled)
	{
		float dt = Time::DeltaTime();
		ElapsedTime += dt;
		s_timePenaltyAccum += dt;

		// 1秒ごとに スコア -10
		while (s_timePenaltyAccum >= 1.0f)
		{
			Score -= 10;
			if (Score < 0) Score = 0;
			s_timePenaltyAccum -= 1.0f;
		}
	}

	// ==== プレイヤー死亡チェック → 残機＆リトライ処理 ====
	Player* p = FindGameObject<Player>();

	if (!deathHandled)
	{
		if (p == nullptr || p->IsDead())
		{
			deathHandled = true;

			// スコア上の死亡処理
			PlayScene::OnPlayerDeath();

			// 残機減らす
			LifeCount--;

			if (LifeCount <= 0)
			{
				// 残機ゼロ → ゲームオーバー
				SceneManager::ChangeScene("GAMEOVER");
			}
			else
			{
				// 残機がまだある → 同じステージやり直し
				SceneManager::ChangeScene("PLAY");
			}
			return;
		}
	}

	// ==== シーン切り替え系の入力 ====
	if (CheckHitKey(KEY_INPUT_F2)) {
		SceneManager::ChangeScene("TITLE");
	}

	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}

	if (CheckHitKey(KEY_INPUT_R)) {
		SceneManager::ChangeScene("STAGE");
	}
}

// Draw（HUD表示）
void PlayScene::Draw()
{
	int white = GetColor(255, 255, 255);
	int h = GetFontSize();

	DrawString(0, 0, "PLAY SCENE", white);

	// タイム・スコア・残機の表示
	DrawFormatString(0, 0 + h * 1, white, "TIME : %5.1f", ElapsedTime);
	DrawFormatString(0, 0 + h * 2, white, "SCORE: %d", Score);
	DrawFormatString(0, 0 + h * 3, white, "LIFE : %d", LifeCount);
}
