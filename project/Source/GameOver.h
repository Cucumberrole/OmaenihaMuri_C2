#pragma once
#include "../Library/SceneBase.h"
#include "../Library/GameObject.h"

class GameOver:public SceneBase
{
public:
	GameOver();
	~GameOver();
	void Update()override;
	void Draw()override;
private:
	int OwariImage;
};

