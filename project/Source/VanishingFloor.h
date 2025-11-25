#pragma once
#include "../Library/GameObject.h"

class VanishingFloor : public GameObject
{
public:
	VanishingFloor(int sx, int sy);
	~VanishingFloor();

	void Update() override;
	void Draw() override;

	// °‚ªÁ‚¦‚Ä‚¢‚È‚¢‚¾‚¯“–‚½‚è”»’è‚ğ•Ô‚·
	bool IsActive() const { return isActive; }

	int HitCheckDown(int px, int py);
	int HitCheckUp(int px, int py);
	int HitCheckLeft(int px, int py);
	int HitCheckRight(int px, int py);

private:
	int hImage;
	float x, y;
	bool isActive;      // true = °‚ª‘¶İ‚·‚é, false = Á‚¦‚½
	float vanishRange;  // ƒvƒŒƒCƒ„[‚ª‹ß‚Ã‚­‹——£
};