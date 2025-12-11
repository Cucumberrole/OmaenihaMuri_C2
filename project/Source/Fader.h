#pragma once
#include "../Library/GameObject.h"

class Fader : public GameObject
{
public:
	Fader();
	~Fader();
	void Update() override;
	void Draw() override;

	void FadeIn(float sec);
	void FadeOut(float sec);
	bool Finished();
	void SetColor(int r, int g, int b);
	
private:
	int alpha;
	int startAlpha;
	int endAlpha;
	float time;
	float now;
	unsigned int rgb;
};

