#pragma once
#include "../Library/GameObject.h"

class Common : public GameObject
{
public:
	Common();
	void Update() override;
	void Draw() override;

	// ‚Ç‚±‚©‚ç‚Å‚à‹¤’Êİ’è‚ğG‚ê‚é‚æ‚¤‚É‚·‚é—p
	static Common* GetInstance();

	int  stageNumber;
	int  score;
	int  hiScore;

	// –³“Gƒtƒ‰ƒO
	bool invincible;

private:
	static Common* s_instance;
};
