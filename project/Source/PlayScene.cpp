#include "PlayScene.h"
#include "Hud.h"

#include <DxLib.h>
#include "../Library/Time.h"

#include "GameConfig.h"
#include "SelectStage.h"
#include "GameResult.h"
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

PlayScene::PlayScene()
{
	hImage = LoadGraph("data/image/Atama.png");


	heartImage = LoadGraph("data/image/heart.png");
	Hud::SetHeartGraph(heartImage);

	new Background();
	Hud::Init();
	VanishingFloor::ResetAll();

	// ステージ未選択なら強制的に1へ
	if (SelectedStage < 0)
		SelectedStage = 1;

	new Field(SelectedStage);

	sound = 0;
	ChangeVolumeSoundMem(150, StageBGM1);
	ChangeVolumeSoundMem(150, StageBGM2);
	StageBGM1 = LoadSoundMem("data/BGM/Stage1.mp3");
	StageBGM2 = LoadSoundMem("data/BGM/Stage2.mp3");
	LastSE = LoadSoundMem("data/BGM/life_warning.mp3");

	if (SelectedStage == 1)
	{
		PlaySoundMem(StageBGM1, DX_PLAYTYPE_LOOP);
	}
	else if (SelectedStage == 2)
	{
		PlaySoundMem(StageBGM2, DX_PLAYTYPE_LOOP);
	}

	if (!g_IsRetry)
	{
		g_Life = MaxLives();   // Easy=5 / Hard=3
	}
	life = g_Life;
	retryCount = g_RetryCount;
	deathCount = g_deathCount;

	playTime = 0.0f;
	score = 10000;

	state = Playstate::Play;
	deathHandled = false;

	if (!g_IsRetry) {
		g_ClearTimeSeconds = 0.0f;
	}
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

	// クリア時間
	g_ClearTimeSeconds += Time::DeltaTime();

	switch (state)
	{
	case Playstate::Play:
	{
		// プレイ中のみ時間/スコア更新
		playTime += Time::DeltaTime();
		score = 10000 - (int)(playTime) * 10 - retryCount * 500;

		// =========================
		// 死亡判定：トラップ等で死んだら
		// 残機を1減らし、死亡演出を再生
		// =========================
		if (player->IsDead())
		{
			// 死んだ瞬間に残機減算などを行う
			if (!deathHandled)
			{
				deathHandled = true;

				life--;
				retryCount++;
				deathCount++;

				g_Life = life;
				g_RetryCount = retryCount;
				g_deathCount = deathCount;
				StopSoundMem(StageBGM1);
				StopSoundMem(StageBGM2);
				
			}

			// 死亡演出中へ（入力は遮断）
			state = Playstate::Death;
			return;
		}

		// =========================
		// クリア判定（生存中のみ）
		// =========================
		if (field->IsGoal((int)(player->GetX() + 32), (int)(player->GetY() + 32)))
		{
			// クリアタイムは「全体時間」を採用
			const float clearTime = g_ClearTimeSeconds;

			// （あなたが新GameResult方式なら）
			GR_FixOnGoalOnce_Manual((int)clearTime, retryCount);

			SceneManager::ChangeScene("CLEAR");
		
			return;
		}

		// =========================
		// 生存中の入力（デバッグ含む）
		// =========================
		if (CheckHitKey(KEY_INPUT_K)) fader->FadeIn(0.5f);
		if (CheckHitKey(KEY_INPUT_L)) fader->FadeOut(1.0f);

		if (CheckHitKey(KEY_INPUT_O) || CheckHitKey(KEY_INPUT_T)) SceneManager::ChangeScene("TITLE");
		if (CheckHitKey(KEY_INPUT_E)) SceneManager::ChangeScene("STAGE");
		if (CheckHitKey(KEY_INPUT_G)) SceneManager::ChangeScene("GAMEOVER");
		if (CheckHitKey(KEY_INPUT_C)) SceneManager::ChangeScene("CLEAR");

		// 生存中Rリトライ
		if (CheckHitKey(KEY_INPUT_R))
		{

			fader->FadeOut(0.5f);
			fader->FadeIn(1.0f);

			g_IsRetry = true;
			SceneManager::ForceChangeScene("PLAY");
		}

		if (CheckHitKey(KEY_INPUT_ESCAPE)) SceneManager::Exit();

		return;
	}

	case Playstate::Death:
	{
		// 死亡演出が終わるまで待機（入力不可）
		if (!player->IsdeathAnimEnd())
		{
			return;
		}

		// 演出終了後、残機が0以下ならゲームオーバーへ
		if (life <= 0)
		{
			// 次回プレイ用に初期化
			g_Life = MaxLives();
			SceneManager::ChangeScene("GAMEOVER");
			return;
		}

		// 残機が残っているなら残機表示へ
		state = Playstate::Zanki;
		if (life == 1)
		{
			PlaySoundMem(LastSE, DX_PLAYTYPE_BACK);
		}
		StopSoundMem(LastSE);
		return;
	}

	case Playstate::Zanki:
	{
		
		
		// 黒画面中はRだけ受付
		if (KeyTrigger::CheckTrigger(KEY_INPUT_R))
		{
			VanishingFloor::ResetAll();

			fader->FadeOut(0.5f);
			fader->FadeIn(1.0f);

			// リトライ
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

	// HUD
	// スコア
	const int hudScore = score;

	// 時間
	const int hudTimeSeconds = (int)g_ClearTimeSeconds;

	// 3) ライフ
	const int hudLife = life;

	// HUD 描画
	Hud::Draw(hudScore, hudTimeSeconds, hudLife);

	// FPS
	//DrawFormatString(10, 120, GetColor(255, 255, 255), "%4.1f", 1.0f / Time::DeltaTime());
}


int PlayScene::GetRetry(int retry)
{
	retry = g_deathCount;
	return retry;
}

