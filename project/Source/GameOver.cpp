#include "GameOver.h"
#include "Screen.h"
#include "Player.h"
#include "PlayScene.h"
#include <ctime>

GameOver::GameOver()
{
	srand((unsigned)time(NULL));
	Rand = rand() % 9 + 1;
	OwariImage = LoadGraph("data/Font/GAME OVER.png");//画像変えるときここ
	oneLineMsg = LoadGraph("data/font/ゲームオーバー画面文字/初のゲームオーバー.png");
	MsgImage1 = LoadGraph("data/font/ゲームオーバー画面文字/この死が君を強くする.png");//
	MsgImage2 = LoadGraph("data/font/ゲームオーバー画面文字/この死は意味があったのか.png");//
	MsgImage3 = LoadGraph("data/font/ゲームオーバー画面文字/ドンマイ！元気出して.png");//
	MsgImage4 = LoadGraph("data/font/ゲームオーバー画面文字/過去の屍を越えてゆけ.png");//
	MsgImage5 = LoadGraph("data/font/ゲームオーバー画面文字/完.png");//
	MsgImage6 = LoadGraph("data/font/ゲームオーバー画面文字/泣かないで….png");
	MsgImage7 = LoadGraph("data/font/ゲームオーバー画面文字/君はキャラの気持ちを考えたことはあるかい？.png");//
	MsgImage8 = LoadGraph("data/font/ゲームオーバー画面文字/止まるんじゃねぇぞ・・・.png");//
	MsgImage9 = LoadGraph("data/font/ゲームオーバー画面文字/疲れてるんじゃない？.png");//

}

GameOver::~GameOver()
{
}

void GameOver::Update()
{
	PlayScene* playscene = FindGameObject<PlayScene>();

	retry = playscene->GetRetry(retry);
	if (CheckHitKey(KEY_INPUT_T)) {
		SceneManager::ChangeScene("TITLE");
	}
	if (CheckHitKey(KEY_INPUT_R)) {
		SceneManager::ChangeScene("PLAY");
	}
	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}

	Msg = oneLineMsg;
	if (retry > 1)
	{
		if (Rand == 1) {
			Msg = MsgImage1;
		}
		if (Rand == 2) {
			Msg = MsgImage2;
		}
		if (Rand == 3) {
			Msg = MsgImage3;
		}
		if (Rand == 4) {
			Msg = MsgImage4;
		}
		if (Rand == 5) {
			Msg = MsgImage5;
		}
		if (Rand == 6) {
			Msg = MsgImage6;
		}
		if (Rand == 7) {
			Msg = MsgImage7;
		}
		if (Rand == 8) {
			Msg = MsgImage8;
		}
		if (Rand == 9) {
			Msg = MsgImage9;
		}
	}
}

void GameOver::Draw()
{


	DrawBox(0, 0, Screen::WIDTH, Screen::HEIGHT,
		GetColor(0, 0, 0), TRUE);
	DrawGraph(600, 200, OwariImage, TRUE);
	

	SetFontSize(25);
	int h = GetFontSize();
	extern const char* Version();

	DrawString(0, 0 + h * 0, "GameOver", GetColor(255, 255, 255));
	DrawString(0, 0 + h * 1, Version(), GetColor(255, 255, 255));
	DrawFormatString(0, 0 + h * 2, GetColor(255, 255, 255), "%4.1f", 1.0f / Time::DeltaTime());
	DrawFormatString(0, 0 + h * 3, GetColor(255, 255, 255), "FontSize:%d", h);
	DrawFormatString(0, 0 + h * 4, GetColor(255, 255, 255), "FontSize:%d", Rand);
	SetFontSize(32);

	DrawBox(700, 675, 1150, 775, GetColor(255, 163, 30), TRUE);
	DrawBox(700, 850, 1150, 950, GetColor(255, 163, 30), TRUE);
	DrawGraph(250, 150, Msg, TRUE);
	int DrawWidth = GetDrawStringWidth("Push [SHIFT]Key To Play", -1);
	DrawString((Screen::WIDTH - DrawWidth) / 2, Screen::HEIGHT / 2+100, "Rキーでリトライ", GetColor(255, 255, 255));
	DrawString((Screen::WIDTH - DrawWidth) / 2, Screen::HEIGHT / 2+275, "Tキーでタイトルに戻る", GetColor(255, 255, 255));


}
