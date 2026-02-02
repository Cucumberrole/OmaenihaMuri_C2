#pragma once
#include "../Library/SceneBase.h"
#include "../Library/GameObject.h"

class GameOver :public SceneBase
{
public:
	GameOver();
	~GameOver();
	void Update()override;
	void Draw()override;
private:
	int OwariImage;
	int BackImage;
	int oneLineMsg;
	int retry;
	int Msg;
	int Rand;
	//àÍåæâÊëú
	int MsgImage1;
	int MsgImage2;
	int MsgImage3;
	int MsgImage4;
	int MsgImage5;
	int MsgImage6;
	int MsgImage7;
	int MsgImage8;
	int MsgImage9;
	
	int GameOverBGM;
	int lastRetry = -1;

	// Ç†Ç…ÇﬂÅ[ÇµÇÂÇÒÅI
	int startMs_ = 0;
	float t_ = 0.0f;           // seconds
	int fadeA_ = 0;            // 0..255
	float titlePop_ = 1.0f;    // scale
	float msgPop_ = 1.0f;      // scale
	float shake_ = 0.0f;       // pixels
	float btnPulse_ = 0.0f;    // 0..1
};