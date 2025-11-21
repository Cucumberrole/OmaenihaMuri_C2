#include "PlayScene.h"
#include "Field.h"
#include "Player.h"
#include "Background.h"
#include "FallingFloor.h"
#include <iostream>

PlayScene::PlayScene()
{
	new Background();
	new Field(8);
}

PlayScene::~PlayScene()
{
}

void PlayScene::Update()
{
	if (CheckHitKey(KEY_INPUT_F2)) {
		SceneManager::ChangeScene("TITLE");
	}

	if (CheckHitKey(KEY_INPUT_G)) {
		SceneManager::ChangeScene("GAMEOVER");
	}

	if (CheckHitKey(KEY_INPUT_H)) {
		SceneManager::ChangeScene("CLEAR");
	}

	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}
}

void PlayScene::Draw()
{
	SetFontSize(20);
	int h = GetFontSize();
	DrawString(0, 0 + h * 0, "PLAY SCENE", GetColor(255, 255, 255));
	DrawFormatString(0, 0 + h * 1, GetColor(255, 255, 255), "%4.1f", 1.0f / Time::DeltaTime());

	DrawString(100, 400, "Push [F2]Key To Title", GetColor(255, 255, 255));
}
