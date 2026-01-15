#pragma once
#include "../Library/GameObject.h"

// j‚ÌŒü‚«
enum class SmallSpikeDir
{
	Up,
	Down,
	Left,
	Right
};

class SmallTrap : public GameObject
{
public:
	SmallTrap(float sx, float sy, SmallSpikeDir dir);
	~SmallTrap();
	void Update() override;
	void Draw() override;
private:
	float x = 0.0f;
	float y = 0.0f;
	int   width = 16;
	int   height = 16;

	SmallSpikeDir dir;

	int SImage = -1; // ‰æ‘œ1–‡‚ÅÏ‚Ü‚¹‚é‚È‚ç‚±‚ê‚¾‚¯
};