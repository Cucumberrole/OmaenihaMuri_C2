#include "PlayScene.h"
#include "Field.h"
#include "Background.h"
#include "../Library/SceneManager.h"
#include "../Library/Trigger.h"

int PlayScene::SelectedStage = -1;

PlayScene::PlayScene()
{
	new Background();

	// ステージ未選択なら強制的に1へ
	if (SelectedStage < 0)
		SelectedStage = 1;

	new Field(SelectedStage);

	sound = 0;
	Ssound = LoadSoundMem("data/sound/bgm_ogg.ogg");
}

PlayScene::~PlayScene()
{
	InitSoundMem();
}

void PlayScene::Update()
{
	if (CheckHitKey(KEY_INPUT_F2)) {
		SceneManager::ChangeScene("TITLE");
	}

	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}

	// --- Rキーでリトライ ---
	if (KeyTrigger::CheckTrigger(KEY_INPUT_R))
	{
		SceneManager::ChangeScene("STAGE");
		return;
	}
}

void PlayScene::Draw()
{
	DrawString(0, 0, "PLAY SCENE", GetColor(255, 255, 255));
}