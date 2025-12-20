#include "GameMain.h"
#include "FontUtil.h"
#include <DxLib.h>

GameMain* GameMain::s_instance = nullptr;

GameMain::GameMain()
{
	DontDestroyOnSceneChange();
	s_instance = this;

	// ttf を読み込む
	// data/font/MyFont.ttf をプロジェクトに置いている想定
	FontUtil::LoadPrivateFont(L"data/Font/x10y12pxDonguriDuel.ttf");

	// フォントハンドル作成
	// 「フォント名」は ttf の中のファミリ名（Windowsのフォントビューアで確認）
	hTitleFont = CreateFontToHandle(
		"x10y12pxDonguriDuel",   // ここを実際のフォント名に
		64,                   // サイズ
		6,                    // 太さ
		DX_FONTTYPE_ANTIALIASING_8X8
	);
}

GameMain::~GameMain()
{
	if (hTitleFont != -1)
	{
		DeleteFontToHandle(hTitleFont);
		hTitleFont = -1;
	}

	FontUtil::UnloadPrivateFont(L"data/Font/x10y12pxDonguriDuel.ttf");
}
