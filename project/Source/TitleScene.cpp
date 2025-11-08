#include "TitleScene.h"

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Update()
{
	if (CheckHitKey(KEY_INPUT_LSHIFT) || CheckHitKey(KEY_INPUT_RSHIFT)) {
		SceneManager::ChangeScene("PLAY");
	}
	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}
}


void TitleScene::Draw()
{
	int h = GetFontSize();
	extern const char* Version();

	DrawString(0, 0 + h * 0, "TITLE SCENE", GetColor(255, 255, 255));
	DrawString(0, 0 + h * 1, Version(), GetColor(255, 255, 255));
	DrawFormatString(0, 0 + h * 2, GetColor(255, 255, 255), "%4.1f", 1.0f / Time::DeltaTime());
	DrawString(0, 0 + h * 3, "FontSize:", GetColor(255, 255, 255));
	DrawString(100, 400, "Push [SHIFT]Key To Play", GetColor(255, 255, 255));

}
