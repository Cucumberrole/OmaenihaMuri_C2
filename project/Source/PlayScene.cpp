#include "PlayScene.h"
#include "Hud.h"

#include <DxLib.h>
#include <cmath>

#include "../Library/Time.h"

#include "GameConfig.h"
#include "GameResult.h"
#include "SelectStage.h"
#include "Player.h"
#include "Field.h"
#include "Fader.h"
#include "Background.h"
#include "VanishingFloor.h"
#include "../Library/SceneManager.h"
#include "../Library/Trigger.h"

int PlayScene::SelectedStage = -1;
static int g_Life = 0;          // 初期
static int g_RetryCount = 0;    // 死んだ回数
float g_ClearTimeSeconds = 0.0f;
static int g_deathCount = 0;
static bool g_IsRetry = false;

static CourseType ToCourseType(Difficulty d)
{
	return (d == Difficulty::Easy) ? CourseType::Easy : CourseType::Hard;
}

PlayScene::PlayScene()
{
	hImage = LoadGraph("data/image/Atama.png");

	heartImage = LoadGraph("data/image/heart.png");
	Hud::SetHeartGraph(heartImage);

	new Background();
	Hud::Init();
	VanishingFloor::ResetAll();

	if (SelectedStage < 0) SelectedStage = 1;
	new Field(SelectedStage);

	// BGM/SE
	StageBGM1 = LoadSoundMem("data/BGM/Stage1.mp3");
	StageBGM2 = LoadSoundMem("data/BGM/Stage2.mp3");
	LastSE = LoadSoundMem("data/BGM/life_warning.mp3");

	if (StageBGM1 >= 0) ChangeVolumeSoundMem(150, StageBGM1);
	if (StageBGM2 >= 0) ChangeVolumeSoundMem(150, StageBGM2);

	if (SelectedStage == 1 && StageBGM1 >= 0) PlaySoundMem(StageBGM1, DX_PLAYTYPE_LOOP);
	if (SelectedStage == 2 && StageBGM2 >= 0) PlaySoundMem(StageBGM2, DX_PLAYTYPE_LOOP);

	// 新規開始時だけ初期化
	if (!g_IsRetry)
	{
		g_Life = MaxLives();
		g_RetryCount = 0;
		g_deathCount = 0;

		GR_ResetRun(ToCourseType(SelectedDifficulty()));
	}

	life = g_Life;
	retryCount = g_RetryCount;
	deathCount = g_deathCount;

	state = Playstate::Play;
	deathHandled = false;

	g_IsRetry = false;
}

PlayScene::~PlayScene()
{
	Hud::Shutdown();
	InitSoundMem();

}

void PlayScene::Update()
{
	Player* player = FindGameObject<Player>();
	Field* field = FindGameObject<Field>();
	Fader* fader = FindGameObject<Fader>();
	if (!player || !field || !fader) return;

	switch (state)
	{
	case Playstate::Play:
	{
		const float dt = Time::DeltaTime();

		// プレイ中だけ GameResult 側の時間減点を進める
		GR_UpdateDuringPlay();

		// 現在スコア（liveScore）をHUD用に取得
		score = GR_GetLiveScore();


		// =========================
		// 死亡判定
		// =========================
		if (player->IsDead())
		{
			if (!deathHandled)
			{
				deathHandled = true;

				GR_OnDeath();

				GR_PauseTimer();

				life--;
				retryCount++;
				deathCount++;

				g_Life = life;
				g_RetryCount = retryCount;
				g_deathCount = deathCount;
				StopSoundMem(StageBGM1);
				StopSoundMem(StageBGM2);

			}

			state = Playstate::Death;
			return;
		}

		// =========================
		// クリア判定
		// =========================
		if (field->IsGoal((int)(player->GetX() + 32), (int)(player->GetY() + 32)))
		{
			GR_FixOnGoalOnce();
			SceneManager::ChangeScene("CLEAR");
			return;
		}


		// 入力など
		if (CheckHitKey(KEY_INPUT_E)) SceneManager::ChangeScene("STAGE");
		if (CheckHitKey(KEY_INPUT_G)) SceneManager::ChangeScene("GAMEOVER");
		if (CheckHitKey(KEY_INPUT_C)) SceneManager::ChangeScene("CLEAR");
		if (CheckHitKey(KEY_INPUT_O) || CheckHitKey(KEY_INPUT_T)) SceneManager::ChangeScene("TITLE");

		if (CheckHitKey(KEY_INPUT_R))
		{
			fader->FadeOut(0.5f);
			fader->FadeIn(1.0f);

			GR_ResumeTimer();
			g_IsRetry = true;
			SceneManager::ForceChangeScene("PLAY");
		}

		if (CheckHitKey(KEY_INPUT_ESCAPE)) SceneManager::Exit();

		return;
	}

	case Playstate::Death:
	{
		if (!player->IsdeathAnimEnd()) return;

		if (life <= 0)
		{
			g_Life = MaxLives();
			SceneManager::ChangeScene("GAMEOVER");
			return;
		}

		state = Playstate::Zanki;
		if (life == 1)
		{
			PlaySoundMem(LastSE, DX_PLAYTYPE_BACK);
		}

		return;
	}

	case Playstate::Zanki:
	{
		if (KeyTrigger::CheckTrigger(KEY_INPUT_R))
		{
			VanishingFloor::ResetAll();

			fader->FadeOut(0.5f);
			fader->FadeIn(1.0f);

			GR_ResumeTimer();
			g_IsRetry = true;
			SceneManager::ForceChangeScene("PLAY");
		}
		return;
	}
	}
}


void PlayScene::Draw()
{
	Player* player = FindGameObject<Player>();

	// 黒画面＋残機表示＋リトライ表示
	if (state == Playstate::Zanki)
	{
		int sw, sh;
		GetDrawScreenSize(&sw, &sh);

		DrawBox(0, 0, sw, sh, GetColor(0, 0, 0), TRUE);

		const char* retryText = "RETRY push to [R]";
		int textWidth = GetDrawStringWidth(retryText, -1);
		int x = (sw - textWidth) / 2;
		int y = sh / 2;

		DrawRotaGraph(x + 40, y, 2.0, 0, hImage, TRUE);
		DrawFormatString(x + 150, y - 10, GetColor(255, 255, 255), "LIFE : %d", life);
		DrawFormatString(x, y + 70, GetColor(255, 255, 255), "%s", retryText);
		return;
	}

	// HUD（GameResultから取得して1回だけ描画）
	const int hudScore = GR_GetLiveScore();
	const int hudTimeSeconds = GR_GetElapsedSecLive();
	const int hudLife = life;

	Hud::Draw(hudScore, hudTimeSeconds, hudLife);

	// FPS（必要なら）
	// DrawFormatString(10, 120, GetColor(255, 255, 255), "%4.1f", 1.0f / Time::DeltaTime());
}

int PlayScene::GetRetry(int retry)
{
	retry = g_deathCount;
	return retry;
}