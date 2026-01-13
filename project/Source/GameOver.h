#pragma once
#include "../Library/SceneBase.h"
#include "../Library/GameObject.h"

class GameOver:public SceneBase
{
public:
	GameOver();
	~GameOver();
	void Update()override;
	void Draw()override;
private:
	int OwariImage;
	int BackImage;
	std::string oneLineMsg;
	int retry;
	int Msg;
	int Rand;
	//ˆêŒ¾‰æ‘œ
	int MsgImage1;
	int MsgImage2;
	int MsgImage3;
	int MsgImage4;
	int MsgImage5;
	int MsgImage6;
	int MsgImage7;
	int MsgImage8;
	int MsgImage9;



};

