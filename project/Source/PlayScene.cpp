#include "PlayScene.h"
#include "Field.h"
#include "Background.h"
#include "../Library/SceneManager.h"
#include "../Library/Trigger.h"

int PlayScene::SelectedStage = 0;

PlayScene::PlayScene()
{
	new Background();
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
}

void PlayScene::Draw()
{
	DrawString(20, 20, "PLAY SCENE", GetColor(255, 255, 255));
}
