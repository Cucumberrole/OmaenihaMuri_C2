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
	if (retry >= 6)
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
    int sw, sh;
    GetDrawScreenSize(&sw, &sh);

    DrawBox(0, 0, sw, sh, GetColor(0, 0, 0), TRUE);

    int titleW = 0, titleH = 0;
    GetGraphSize(OwariImage, &titleW, &titleH);

    int msgW = 0, msgH = 0;
    GetGraphSize(Msg, &msgW, &msgH);

    const int marginTop = sh / 12;
    const int gap1 = sh / 30;
    const int gap2 = sh / 18;

    int titleX = (sw - titleW) / 2;
    int titleY = marginTop;
    DrawGraph(titleX, titleY, OwariImage, TRUE);

    int msgX = (sw - msgW) / 2;
    int msgY = titleY + titleH + gap1;
    DrawGraph(msgX, msgY, Msg, TRUE);

    const int buttonW = (sw < 1200) ? (sw * 2 / 3) : 520;
    const int buttonH = 90;
    const int buttonGap = 30;

    int buttonX = (sw - buttonW) / 2;
    int buttonY1 = msgY + msgH + gap2;
    int buttonY2 = buttonY1 + buttonH + buttonGap;

    int bottom = buttonY2 + buttonH + marginTop / 2;
    if (bottom > sh)
    {
        int pullUp = bottom - sh;
        buttonY1 -= pullUp;
        buttonY2 -= pullUp;
    }

    const int btnColor = GetColor(255, 163, 30);
    DrawBox(buttonX, buttonY1, buttonX + buttonW, buttonY1 + buttonH, btnColor, TRUE);
    DrawBox(buttonX, buttonY2, buttonX + buttonW, buttonY2 + buttonH, btnColor, TRUE);

    SetFontSize(32);
    const int fontH = GetFontSize();

    auto DrawCenteredTextInBox = [&](int x1, int y1, int x2, int y2, const char* text)
        {
            int tw = GetDrawStringWidth(text, -1);
            int tx = x1 + ((x2 - x1) - tw) / 2;
            int ty = y1 + ((y2 - y1) - fontH) / 2;
            DrawString(tx, ty, text, GetColor(255, 255, 255));
        };


    DrawCenteredTextInBox(buttonX, buttonY1, buttonX + buttonW, buttonY1 + buttonH, "Rキーでリトライ");
    DrawCenteredTextInBox(buttonX, buttonY2, buttonX + buttonW, buttonY2 + buttonH, "Tキーでタイトルへ戻る");

    SetFontSize(20);
    DrawString(20, sh - 40, "ESC : Exit", GetColor(180, 180, 180));
}
