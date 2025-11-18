#pragma once
#include "../Library/SceneBase.h"

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

