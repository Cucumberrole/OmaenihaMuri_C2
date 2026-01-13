#pragma once
#include "../Library/GameObject.h"

// j‚ÌŒü‚«
enum class SpikeDir
{
	Up,
	Down,
	Left,
	Right
};

class DirectionalSpike : public GameObject
{
public:
	DirectionalSpike(float sx, float sy, SpikeDir dir);
	~DirectionalSpike();

	void Update() override;
	void Draw() override;

private:
	float x = 0.0f;
	float y = 0.0f;

	int   width = 64;
	int   height = 64;

	SpikeDir dir;

	int hImage = -1; // ‰æ‘œ1–‡‚ÅÏ‚Ü‚¹‚é‚È‚ç‚±‚ê‚¾‚¯
};
