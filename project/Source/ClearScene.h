#pragma once
#include "../Library/SceneBase.h"

/// <summary>
/// タイトルシーン
/// 
/// タイトルを表示して、キーを押したらプレイシーンに移行する。
/// </summary>
class ClearScene : public SceneBase
{
public:
	ClearScene();
	~ClearScene();
	void Update() override;
	void Draw() override;
};
