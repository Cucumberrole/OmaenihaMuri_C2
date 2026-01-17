#include "PlayScene.h"

#include <DxLib.h>
#include "../Library/Time.h"

#include "GameResult.h"
#include "Player.h"
#include "Field.h"
#include "Fader.h"
#include "Background.h"
#include "VanishingFloor.h"
#include "../Library/SceneManager.h"
#include "../Library/Trigger.h"

int PlayScene::SelectedStage = -1;
static int g_Life = 5;          // 初期残機
static int g_RetryCount = 0;    // 死んだ回数
float g_ClearTimeSeconds = 0.0f;
static int g_deathCount = 0;

PlayScene::PlayScene()
{
	hImage = LoadGraph("data/image/Atama.png");

	new Background();

	// ステージ未選択なら強制的に1へ
	if (SelectedStage < 0)
		SelectedStage = 1;

	new Field(SelectedStage);

	sound = 0;
	Ssound = LoadSoundMem("data/sound/bgm_ogg.ogg");

	life = g_Life;
	retryCount = g_RetryCount;
	deathCount = g_deathCount;

	playTime = 0.0f;
	score = 10000;

	state = Playstate::Play;
	deathHandled = false;
}

PlayScene::~PlayScene()
{
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
			int finalScore = score;
			if (retryCount == 0)   finalScore += 2000;
			if (playTime <= 60.0f) finalScore += 1000;

			g_GameResult.score = finalScore;
			g_GameResult.clearTime = playTime;
			g_GameResult.retryCount = retryCount;

			SceneManager::ChangeScene("CLEAR");
			return;
		}

		// =========================
		// 生存中の入力（デバッグ含む）
		// =========================
		if (CheckHitKey(KEY_INPUT_K)) fader->FadeIn(0.5f);
		if (CheckHitKey(KEY_INPUT_L)) fader->FadeOut(1.0f);

		if (CheckHitKey(KEY_INPUT_O)) SceneManager::ChangeScene("TITLE");
		if (CheckHitKey(KEY_INPUT_E)) SceneManager::ChangeScene("STAGE");
		if (CheckHitKey(KEY_INPUT_G)) SceneManager::ChangeScene("GAMEOVER");

		// 生存中Rリトライ
		if (CheckHitKey(KEY_INPUT_R))
		{
			VanishingFloor::ResetAll();
			fader->FadeOut(0.5f);
			fader->FadeIn(1.0f);
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
			g_Life = 5;
			SceneManager::ChangeScene("GAMEOVER");
			return;
		}

		// 残機が残っているなら残機表示へ
		state = Playstate::Zanki;
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

		// 表示文字（中央配置）
		const char* retryText = "リトライ Push to [R]";
		int textWidth = GetDrawStringWidth(retryText, -1);
		int x = (sw - textWidth) / 2;
		int y = sh / 2;

		DrawRotaGraph(x + 40, y, 2.0, 0, hImage, TRUE);
		DrawFormatString(x + 150, y - 10, GetColor(255, 255, 255), "　残機　 %d", life);
		DrawFormatString(x, y + 70, GetColor(255, 255, 255), "%s", retryText);
		return;
	}

	// 通常表示（デバッグ表示）
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

	// PlayerX/Y
	if (player)
	{
		DrawFormatString(0, 0 + h * 3, GetColor(255, 255, 255), "PlayerX: %.2f", player->GetX());
		DrawFormatString(0, 0 + h * 4, GetColor(255, 255, 255), "PlayerY: %.2f", player->GetY());
	}

	// LIFE
	sprintf_s(buf, "LIFE : %d", life);
	DrawString(0, 0 + h * 5, buf, col);

	DrawFormatString(0, 0 + h * 6, GetColor(255, 255, 255), "%4.1f", 1.0f / Time::DeltaTime());
}

int PlayScene::GetRetry(int retry)
{
	retry = g_deathCount;
	return retry;
}
