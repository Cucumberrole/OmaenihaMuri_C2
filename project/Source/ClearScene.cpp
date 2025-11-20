#include "ClearScene.h"
#include "Screen.h"

ClearScene::ClearScene()
{
	ClearImage = LoadGraph("data/image/culear.png");//‰æ‘œ•Ï‚¦‚é‚Æ‚«‚±‚±
}

ClearScene::~ClearScene()
{
}

void ClearScene::Update()
{
	if (CheckHitKey(KEY_INPUT_LSHIFT) || CheckHitKey(KEY_INPUT_RSHIFT)) {
		SceneManager::ChangeScene("TITLE");
	}
	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}
}


void ClearScene::Draw()
{
	DrawGraph(0, 0, ClearImage, FALSE);

	SetFontSize(25);
	int h = GetFontSize();
	extern const char* Version();

	DrawString(0, 0 + h * 0, "CLEAR", GetColor(0, 255, 255));
	DrawString(0, 0 + h * 1, Version(), GetColor(0, 255, 255));
	DrawFormatString(0, 0 + h * 2, GetColor(0, 255, 255), "%4.1f", 1.0f / Time::DeltaTime());
	DrawFormatString(0, 0 + h * 3, GetColor(0, 255, 255), "FontSize:%d", h);


	int DrawWidth = GetDrawStringWidth("Push [SHIFT]Key To Play", -1);
	DrawString((Screen::WIDTH - DrawWidth) / 2, Screen::HEIGHT / 2, "Push [SHIFT]Key To Play", GetColor(0, 255, 255));

}
