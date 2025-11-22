#include "TitleScene.h"
#include "Screen.h"



TitleScene::TitleScene()
{
	TitleImage = LoadGraph("data/image/title.png");//‰æ‘œ•Ï‚¦‚é‚Æ‚«‚Í‚±‚±

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