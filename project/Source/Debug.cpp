#include "Debug.h"
#include "Player.h"
#include "Screen.h"
using namespace Screen;

Debug::Debug()
{
	isDebugMode = false;
	horizontalCount = WIDTH / 64;
	verticalCount = HEIGHT / 64;
	x = 0;
	y = 0;
	maxCount = 15;
}

Debug::~Debug()
{

}

void Debug::Update()
{
	if (CheckHitKey(KEY_INPUT_RCONTROL)) {
		if (pressCounts[D_RCTRL] == 0) return;
		pressCounts[D_RCTRL] = 0;

		isDebugMode = !isDebugMode;
		Player* player = FindGameObject<Player>();
		if (player != nullptr) {
			x = player->GetX() / 64;
			y = player->GetY() / 64;
		}
	}
	else {
		pressCounts[D_RCTRL] = 1;
	}

	if (!isDebugMode) return;

	if (CheckHitKey(KEY_INPUT_RIGHT))
	{
		if (pressCounts[D_RIGHT] >= maxCount) {
			x++;
			pressCounts[D_RIGHT] = 0;
		}
		else {
			pressCounts[D_RIGHT]++;
		}
	}
	else { pressCounts[D_RIGHT] = 15; }
	if (CheckHitKey(KEY_INPUT_LEFT)) 
	{
		if (pressCounts[D_LEFT] >= maxCount) {
			x--;
			pressCounts[D_LEFT] = 0;
		}
		else {
			pressCounts[D_LEFT]++;
		}
	}
	else { pressCounts[D_LEFT] = 15; }
	if (CheckHitKey(KEY_INPUT_UP)) 
	{
		if (pressCounts[D_UP] >= maxCount) {
			y--;
			pressCounts[D_UP] = 0;
		}
		else {
			pressCounts[D_UP]++;
		}
	}
	else { pressCounts[D_UP] = 15; }
	if (CheckHitKey(KEY_INPUT_DOWN)) 
	{
		if (pressCounts[D_DOWN] >= maxCount) {
			y++;
			pressCounts[D_DOWN] = 0;
		}
		else {
			pressCounts[D_DOWN]++;
		}
	}
	else { pressCounts[D_DOWN] = 15; }

	if (CheckHitKey(KEY_INPUT_RETURN)) {
		isDebugMode = false;
		Player* player = FindGameObject<Player>();
		int currentPosX = player->GetX();
		int currentPosY = player->GetY();
		int movedPosX = x * 64;
		int movedPosY = y * 64 -1;
		

		if (player != nullptr) {
			player->Move(movedPosX - currentPosX, movedPosY - currentPosY);
		}

	}
}

void Debug::Draw()
{
	//DrawFormatString(0, 500, GetColor(255, 255, 255), "RIGHT::%d", pressCounts[D_RIGHT]);
	if (!isDebugMode) return;
	for (int x = 0; x < horizontalCount; x++) {
		DrawLine(x * 64, 0, x * 64, HEIGHT, GetColor(0, 255, 0), 2);
	}
	for (int y = 0; y < verticalCount; y++) {
		DrawLine(0, y * 64, WIDTH, y * 64, GetColor(0, 255, 0), 2);
	}

	//SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
	//DrawBox(0, 0, WIDTH, HEIGHT, GetColor(255, 255, 255), TRUE);
	DrawBox(x * 64, y * 64, x * 64 + 64, y * 64 + 64, GetColor(255, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
}
