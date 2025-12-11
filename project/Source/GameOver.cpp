#include "GameOver.h"
#include "Screen.h"
#include "Player.h"


GameOver::GameOver()
{
	OwariImage = LoadGraph("data/image/you lose.png");//画像変えるときここ

}

GameOver::~GameOver()
{
}

void GameOver::Update()
{
	if (CheckHitKey(KEY_INPUT_T)) {
		SceneManager::ChangeScene("TITLE");
	}
	if (CheckHitKey(KEY_INPUT_R)) {
		SceneManager::ChangeScene("PLAY");
	}
	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}


}

void GameOver::Draw()
{

	DrawGraph(0, 0, OwariImage, FALSE);

	SetFontSize(25);
	int h = GetFontSize();
	extern const char* Version();

	DrawString(0, 0 + h * 0, "GameOver", GetColor(0, 0, 0));
	DrawString(0, 0 + h * 1, Version(), GetColor(0, 0, 0));
	DrawFormatString(0, 0 + h * 2, GetColor(0, 0, 0), "%4.1f", 1.0f / Time::DeltaTime());
	DrawFormatString(0, 0 + h * 3, GetColor(0, 0, 0), "FontSize:%d", h);




	int DrawWidth = GetDrawStringWidth("Push [SHIFT]Key To Play", -1);
	DrawString((Screen::WIDTH - DrawWidth) / 2, Screen::HEIGHT / 2, "Tキーでタイトルに戻る\nRキーでリトライする", GetColor(0, 0, 0));


}
