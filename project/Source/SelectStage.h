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
	int selectSE;
	int sHandle;

	// アニメーション用
	float cursorBlinkFrame;   // セレクト枠点滅
	float subTextAnimFrame;   // 上の説明文アニメ


	bool deciding;       // 決定演出中かどうか
	int  decideTimer;    // 決定演出の経過フレーム
	int  decideStageId;  // 決定したステージID
	int  decideSE;       // 決定音SE

	// 選択肢（増やしたいときは cpp 側の options に追加する）
	struct Option
	{
		std::string title;     // 大見出し
		std::string sub1;      // 2行目
		std::string sub2;      // 3行目
		int lives;             // 残機数（表示用）
		int stageId;           // PlayScene::SelectedStage に入れる値
		int hotKey;            // 直接選択キー（KEY_INPUT_E など）
		int boxColor;          // 枠内の色
	};

	std::vector<Option> options;
	int cursor;

	bool debugUnlocked;
};