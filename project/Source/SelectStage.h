#pragma once
#include "../Library/SceneBase.h"
#include <vector>
#include <string>

class SelectStage : public SceneBase
{
public:
	SelectStage();
	~SelectStage();

	void Update() override;
	void Draw() override;

private:
	struct Option
	{
		std::string title;
		std::string sub1;
		std::string sub2;
		int lives = 5;
		int stageId = 1;
		int hotKey = 0;     // KEY_INPUT_***
		int boxColor = 0;   // GetColor(...)
	};

	std::vector<Option> options;
	int cursor = 0;
};