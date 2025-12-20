#pragma once
#include "../Library/GameObject.h"

class LaserTurret : public GameObject
{
public:
	enum class Dir { Up, Down, Left, Right };

	LaserTurret(float sx, float sy, Dir dir);
	~LaserTurret();

	void Update() override;
	void Draw() override;

private:
	float x = 0.0f;
	float y = 0.0f;

	Dir dir;

	int hTurret = -1;

	int animCount = 0;     // アニメ用カウンタ
	int fireInterval = 90;   // 何フレームおきに撃つか
	int beamDuration = 30;   // 何フレームビームを出すか

	float maxLength = 640.0f;
};