#include "PlayScene.h"
#include "Field.h"
#include "Player.h"
#include "Background.h"
#include "FallingFloor.h"
#include "../Library/Trigger.h"
#include <iostream>

PlayScene::PlayScene()
{
	new Background();
	new Field(0);
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
		StopSoundFile();
		SceneManager::ChangeScene("TITLE");
	}

	if (CheckHitKey(KEY_INPUT_G)) {
		StopSoundFile();
		SceneManager::ChangeScene("GAMEOVER");
	}

	if (CheckHitKey(KEY_INPUT_H)) {
		StopSoundFile();
		SceneManager::ChangeScene("CLEAR");
	}

	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}

	if (KeyTrigger::CheckTrigger(KEY_INPUT_B)) {
		if (sound == 0) {
			PlaySoundMem(Ssound, DX_PLAYTYPE_LOOP, TRUE);
			sound = 1;
		}
		else if (sound == 1) {
			StopSoundMem(Ssound);
			sound = 0;
		}
	}
}

void PlayScene::Draw()
{
	SetFontSize(20);
	int h = GetFontSize();
	DrawString(0, 0 + h * 0, "PLAY SCENE", GetColor(255, 255, 255));
	DrawFormatString(0, 0 + h * 1, GetColor(255, 255, 255), "%4.1f", 1.0f / Time::DeltaTime());
	DrawFormatString(0, 0 + h * 2, GetColor(255, 255, 255), "BGM = %d", sound);

	DrawString(100, 400, "Push [F2]Key To Title", GetColor(255, 255, 255));
}
