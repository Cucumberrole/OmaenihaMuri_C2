#pragma once
#include "../Library/GameObject.h"

class LaserTurret : public GameObject
{
public:
	enum class Dir
	{
		Up,
		Down,
		Left,
		Right
	};

	LaserTurret(float sx, float sy, Dir dir);
	~LaserTurret();

	void Update() override;
	void Draw() override;

private:
	float x = 0.0f;
	float y = 0.0f;

	Dir dir;

	int  hTurret = -1;

	// ★ ここにデフォルト値を「直接」入れておく
	int  animCount = 0;
	int  fireInterval = 90;   // 何フレームおきに撃つか
	int  beamDuration = 30;   // 何フレームレーザーを出すか
	float maxLength = 640.0f; // 最大射程
};
