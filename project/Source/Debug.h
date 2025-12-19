#pragma once
#include "../Library/GameObject.h"

class Debug : public GameObject {
public :
	Debug();
	~Debug();
	void Update() override;
	void Draw() override;
private :
	bool isDebugMode;
	int horizontalCount;
	int verticalCount;
	int x;
	int y;

	enum DIRECTION {
		D_UP,
		D_LEFT,
		D_DOWN,
		D_RIGHT,
		D_RCTRL
	};
	int pressCounts[5];
	int maxCount;
};