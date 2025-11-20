#pragma once
#include "../Library/SceneBase.h"

class ClearScene : public SceneBase
{
public:
	ClearScene();
	~ClearScene();
	void Update() override;
	void Draw() override;
private:
	int ClearImage;
};
