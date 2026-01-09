#pragma once
#include "../Library/SceneBase.h"

class PlayScene : public SceneBase
{
public:
	PlayScene();        // ← 引数なし
	~PlayScene();

	static int SelectedStage;  // ← ステージ番号を保持

	void Update() override;
	void Draw() override;

	int hImage;

private:
	int Ssound;
	bool sound;

	int   life;        // 残機 (5スタート)
	bool deathHandled = false;
	int   retryCount;  // 死んだ回数
	float playTime;    // 経過時間(秒)
	int   score;       // 現在スコア

	enum class Playstate {
		Play,
		Death,
		Zanki
	};
	Playstate state;
	int deathTimer;
};