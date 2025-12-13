#include "PlayScene.h"
#include "GameResult.h"
#include "Player.h"
#include "Field.h"
#include "Fader.h"
#include "Background.h"
#include "../Library/SceneManager.h"
#include "../Library/Trigger.h"

int PlayScene::SelectedStage = -1;
static int g_Life = 5; // 初期残機
static int g_RetryCount = 0; // 死んだ回数
float g_ClearTimeSeconds = 0.0f;

PlayScene::PlayScene()
{
	new Background();

	// ステージ未選択なら強制的に1へ
	if (SelectedStage < 0)
		SelectedStage = 1;

	new Field(SelectedStage);

	sound = 0;
	Ssound = LoadSoundMem("data/sound/bgm_ogg.ogg");

	life = g_Life;
	retryCount = g_RetryCount;

	playTime = 0.0f;
	score = 10000;  // 初期スコア
}

PlayScene::~PlayScene()
{
	InitSoundMem();
}

void PlayScene::Update()
{
	playTime += Time::DeltaTime();

	score = 10000 - (int)(playTime) * 10 - retryCount * 500;

	g_ClearTimeSeconds += Time::DeltaTime();

	Player* player = FindGameObject<Player>();
	Field* field = FindGameObject<Field>();
	if (!player || !field) { return; }

	if (player && player->IsDead())
	{
		life--;
		retryCount++;

		g_Life = life;
		g_RetryCount = retryCount;

		if (life > 0)
		{
			SceneManager::ChangeScene("PLAY");
		}
		else {
			SceneManager::ChangeScene("GAMEOVER");
		}
		return;
	}

	if (field->IsGoal((int)(player->GetX() + 32), (int)(player->GetY() + 32)))
	{
		// ボーナス付与
		int finalScore = score;
		if (retryCount == 0) { finalScore += 2000; } // ノーミス
		if (playTime <= 60.0f) { finalScore += 1000; } // 1分以内

		// 結果をグローバルに保存
		g_GameResult.score = finalScore;
		g_GameResult.clearTime = playTime;
		g_GameResult.retryCount = retryCount;

		// CLEARシーンへ
		SceneManager::ChangeScene("CLEAR");
		return;
	}

	if (CheckHitKey(KEY_INPUT_F2)) {
		SceneManager::ChangeScene("TITLE");
	}

	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}

	// --- Rキーでリトライ ---
	if (KeyTrigger::CheckTrigger(KEY_INPUT_R))
	{
		g_RetryCount++;
		SceneManager::ChangeScene("STAGE");
		return;
	}

	// --- フェードインアウト ---
	Fader* fader = FindGameObject<Fader>();
	if (CheckHitKey(KEY_INPUT_I)) {
		fader->FadeIn(0.5f);
	}
	if (CheckHitKey(KEY_INPUT_O)) {
		fader->FadeOut(1.0f);
	}

}

void PlayScene::Draw()
{
	Player* player = FindGameObject<Player>();

	int col = GetColor(255, 255, 255);
	SetFontSize(32);
	int h = GetFontSize();

	char buf[128];
	DrawString(0, 0 + h * 0, "PLAY SCENE", GetColor(255, 255, 255));

	// TIME
	sprintf_s(buf, "TIME : %.2f", playTime);
	DrawString(0, 0 + h * 1, buf, col);

	// SCORE
	sprintf_s(buf, "SCORE : %d", score);
	DrawString(0, 0 + h * 2, buf, col);

	// PlayerX
	DrawFormatString(0, 0 + h * 3, GetColor(255, 255, 255), "PlayerX: %.2f", player->GetX());

	//PlayerY
	DrawFormatString(0, 0 + h * 4, GetColor(255, 255, 255), "PlayerY: %.2f", player->GetY());

	// LIFE
	sprintf_s(buf, "LIFE : %d", life);
	DrawString(0, 0 + h * 5, buf, col);
}