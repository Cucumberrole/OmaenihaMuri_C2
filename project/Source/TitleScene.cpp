#include "TitleScene.h"
#include "PlayScene.h"
#include "GameResult.h"
#include "Screen.h"


TitleScene::TitleScene()
{
	TitleImage = LoadGraph("data/image/title.png");
	sHandle = LoadSoundMem("data/bgm/002.ogg");
	PlaySoundMem(sHandle, DX_PLAYTYPE_BACK);
}

TitleScene::~TitleScene()
{
	DeleteGraph(TitleImage);
	DeleteSoundMem(sHandle);
}

void TitleScene::Update()
{

	if (CheckHitKey(KEY_INPUT_LSHIFT) || CheckHitKey(KEY_INPUT_RSHIFT)) {
		SceneManager::ChangeScene("STAGE");
		g_ClearTimeSeconds = 0.0f;
		g_RetryCount = 0;
	}
	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}

}


void TitleScene::Draw()
{
	DrawGraph(0, 0, TitleImage, FALSE);
	int color = GetColor(0, 0, 0);

	SetFontSize(30);
	int h = GetFontSize();
	extern const char* Version();

	DrawString(0, 0 + h * 0, "TITLE SCENE", color);
	DrawString(0, 0 + h * 1, Version(), color);
	DrawFormatString(0, 0 + h * 2, color, "%4.1f", 1.0f / Time::DeltaTime());
	DrawFormatString(0, 0 + h * 3, color, "FontSize:%d", h);


	int DrawWidth = GetDrawStringWidth("Push [SHIFT]Key To Play", -1);
	DrawString((Screen::WIDTH - DrawWidth) / 2, Screen::HEIGHT / 2, "Push [SHIFT]Key To Play", color);
}