#pragma once
#include "../Library/SceneBase.h"

class PlayScene : public SceneBase
{
public:
	PlayScene();
	~PlayScene();

	static int SelectedStage;

	void Update() override;
	void Draw() override;
	int GetRetry(int retry);

	int hImage;
	int heartImage;

private:
	enum class Playstate
	{
		Play,   // 通常プレイ
		Death,  // 死亡演出中（アニメ終了待ち）
		Zanki   // 黒画面＋残機表示＋R待ち
	};

	Playstate state = Playstate::Play;

	int Ssound;
	bool sound;

	int   life;          // 残機
	int   retryCount;    // リトライ回数（死んだ回数）
	float playTime;      // 経過時間
	int   score;         // スコア
	int   deathCount;    // 死亡回数（保持用）

	// この死亡を処理したか？（残機減算などを一度だけ行う）
	bool deathHandled = false;

	int StageBGM1;
	int StageBGM2;
	int LastSE;
	int RetrySE;
};
