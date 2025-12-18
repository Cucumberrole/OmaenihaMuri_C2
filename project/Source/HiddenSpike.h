#pragma once
#include "../Library/GameObject.h"

class HiddenSpike : public GameObject
{
public:
	HiddenSpike(float sx, float sy, float triggerRange = 160.0f);
	~HiddenSpike();

	void Update() override;
	void Draw() override;

private:
	int   hImage = -1;
	float x = 0.0f;
	float y = 0.0f;

	int   width = 64;
	int   height = 64;

	float triggerRange = 160.0f; // ‹ß‚Ã‚¢‚½‚çŒ©‚¦‚é‹——£
	bool  revealed = false;  // true ‚É‚È‚Á‚½‚çŽp‚ðŒ©‚¹‚é & “–‚½‚è”»’è—LŒø
};