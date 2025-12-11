#include "Fader.h"
#include "Screen.h"

Fader::Fader()
{
	DontDestroyOnSceneChange();
	SetDrawOrder(-10000);
	alpha = 0;
	startAlpha = 0;
	endAlpha = 0;
	time = 0;
	now = 0;
	rgb = 0;//çï
}

Fader::~Fader()
{
}

void Fader::Update()
{
	if (now < time) {
		now += Time::DeltaTime();//1ÉtÉåÅ[ÉÄÇÃïbêî
		float rate = now / time;
		alpha = (endAlpha - startAlpha) * rate + startAlpha;
	}
	else {
		alpha = endAlpha;
	}
}

void Fader::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, Screen::WIDTH, Screen::HEIGHT, rgb, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void Fader::FadeIn(float sec)
{
	startAlpha = 255;
	endAlpha = 0;
	time = sec;
	now = 0;
}

void Fader::FadeOut(float sec)
{
	startAlpha = 0;
	endAlpha = 255;
	time = sec;
	now = 0;
}

bool Fader::Finished()
{
    return now >= time;
}

void Fader::SetColor(int r, int g, int b)
{
	rgb = GetColor(r, g, b);
}
