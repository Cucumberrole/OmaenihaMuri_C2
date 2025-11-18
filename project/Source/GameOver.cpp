#include "GameOver.h"
#include "SceneFactory.h"


GameOver::GameOver()
{
	//OwariImage = LoadGraph("data/image/GAMEOVER.png");
}

GameOver::~GameOver()
{
}

void GameOver::Update()
{
	if (CheckHitKey(KEY_INPUT_T)) {
		SceneManager::ChangeScene("TITLE");
	}
}

void GameOver::Draw()
{
	DrawGraph(200, 100, OwariImage, TRUE);
}
