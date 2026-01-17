#pragma once
#include "../Library/GameObject.h"

class Background : public GameObject
{
public:
	Background();
	~Background();

	void Update() override;
	void Draw() override;

private:
	int bgImage;
	int sw, sh;
};
