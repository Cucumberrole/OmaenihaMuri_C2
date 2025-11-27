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
	int backgroundImage;
	std::vector<bool> stageEnabled;
	std::vector<std::string> stageNames;  // ステージ名一覧
	int cursor;                          // 選択中の番号
};