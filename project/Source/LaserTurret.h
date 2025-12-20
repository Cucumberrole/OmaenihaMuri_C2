#pragma once
#include "../Library/GameObject.h"

class LaserTurret : public GameObject
{
public:
	enum class Dir
	{
		Right,
		Left,
		Up,
		Down
	};

	LaserTurret(float sx, float sy, Dir dir);
	~LaserTurret();

	void Update() override;
	void Draw() override;

private:
	float x = 0.0f;
	float y = 0.0f;

	Dir dir;

	// 砲台（0:通常, 1:発射中）
	int  hHead[2] = { -1, -1 };

	// レーザー画像
	int  hLaser = -1;
	int  laserW = 0;
	int  laserH = 0;

	// タイミング
	int  animCount = 0;
	int  fireInterval = 90;   // 何フレームおきに撃つか
	int  beamDuration = 30;   // 何フレームレーザーを出すか

	// 今フレーム撃っているか
	bool isFiring = false;

	// ビーム情報
	float maxLength = 640.0f;
	float beamStartX = 0.0f;
	float beamStartY = 0.0f;
	float beamEndX = 0.0f;
	float beamEndY = 0.0f;
};
