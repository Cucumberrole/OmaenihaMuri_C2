#pragma once
#include "../Library/SceneBase.h"
#include "../Library/GameObject.h"

class TitleScene : public SceneBase
{
public:
	TitleScene();
	~TitleScene();
	void Update() override;
	void Draw() override;

private:
	// 背景（赤レンガ）
	int TitleImage = -1;

	// タイトルロゴ（背景透過PNG）
	int LogoImage = -1;

	// BGM
	int sHandle = -1;

	// 画像サイズ
	int bgW = 0, bgH = 0;
	int logoW = 0, logoH = 0;

	// バウンス用
	float logoX = 0.0f;
	float logoY = 0.0f;
	float logoVY = 0.0f;
	float logoTargetY = 0.0f;
	bool  logoLanded = false;

	// 点滅用
	float blinkTime = 0.0f;

	// --- パーティクル（火の粉/ほこり） ---
	struct Particle
	{
		float x = 0, y = 0;
		float vx = 0, vy = 0;
		float life = 0;   // 経過
		float ttl = 0;    // 寿命
		float size = 2;   // 半径
		int   kind = 0;   // 0:ほこり 1:火の粉
	};

	static constexpr int kParticleCount = 10;
	Particle particles[kParticleCount];

	void ResetParticle(int i);

};
