#pragma once
#include "../Library/GameObject.h"

class GameMain : public GameObject
{
public:
	GameMain();
	~GameMain();
	void Update() override;
	void Draw() override;

	static GameMain* GetInstance();

	// タイトル用フォント
	int hTitleFont;

	// 無敵フラグなど既存メンバ
	bool invincible;

private:
	static GameMain* s_instance;
};
