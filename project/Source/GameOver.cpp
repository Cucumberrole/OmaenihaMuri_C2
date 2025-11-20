#include "GameOver.h"
#include "SceneFactory.h"
#include "Player.h"


GameOver::GameOver()
{
	//OwariImage = LoadGraph("data/image/    .png");
}

GameOver::~GameOver()
{
}

void GameOver::Update()
{
	if (CheckHitKey(KEY_INPUT_T)) {
		SceneManager::ChangeScene("TITLE");
	}
	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}
}

void GameOver::Draw()
{
	SetFontSize(20);
	int h = GetFontSize();
	DrawString(0, 0 + h * 0, "PLAY SCENE", GetColor(255, 255, 255));
	DrawFormatString(0, 0 + h * 1, GetColor(255, 255, 255), "%4.1f", 1.0f / Time::DeltaTime());
}
