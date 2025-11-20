#include "GameOver.h"
#include "Screen.h"
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
	if (CheckHitKey(KEY_INPUT_LSHIFT) || CheckHitKey(KEY_INPUT_RSHIFT)) {
		SceneManager::ChangeScene("PLAY");
	}
	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}
	
}

void GameOver::Draw()
{
	SetFontSize(25);
	int h = GetFontSize();
	extern const char* Version();

	DrawString(0, 0 + h * 0, "GameOver", GetColor(255, 255, 255));
	DrawString(0, 0 + h * 1, Version(), GetColor(255, 255, 255));
	DrawFormatString(0, 0 + h * 2, GetColor(255, 255, 255), "%4.1f", 1.0f / Time::DeltaTime());
	DrawFormatString(0, 0 + h * 3, GetColor(255, 255, 255), "FontSize:%d", h);


	int DrawWidth = GetDrawStringWidth("Push [SHIFT]Key To Play", -1);
	DrawString((Screen::WIDTH - DrawWidth) / 2, Screen::HEIGHT / 2, "Push [SHIFT]Key To Play", GetColor(255, 255, 255));
}
