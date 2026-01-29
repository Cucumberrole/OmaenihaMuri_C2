#include "PlayScene.h"
#include "Hud.h"

#include <DxLib.h>
#include <cmath>

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
	Ssound = LoadSoundMem("data/sound/bgm_ogg.ogg");

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

    if (!g_IsRetry)
    {
        // ★新規開始時だけ GameResult を初期化して計測開始
        // 難易度の取得方法はあなたのプロジェクトに合わせてください
        // 例：CourseType course = (GameConfig::IsEasy() ? CourseType::Easy : CourseType::Hard);
        CourseType course = CourseType::Easy; // ←ここは実際の難易度取得に置換
        GR_ResetRun(course);
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

                life--;
                retryCount++;
                deathCount++;

                g_Life = life;
                g_RetryCount = retryCount;
                g_deathCount = deathCount;
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
        return;
    }

    case Playstate::Zanki:
    {
        if (KeyTrigger::CheckTrigger(KEY_INPUT_R))
        {
            VanishingFloor::ResetAll();

            fader->FadeOut(0.5f);
            fader->FadeIn(1.0f);

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