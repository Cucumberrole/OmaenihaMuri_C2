#pragma once
#include "../Library/GameObject.h"
#include <DxLib.h>

class Trap : public GameObject
{
public:
	enum class Type { PopUp, Static };
	enum class Dir { Up, Down, Left, Right };

	// Existing: pop-up spike (Up)
	Trap(int x, int y);

	// New: static spike with direction
	Trap(int x, int y, Type type, Dir dir);

	~Trap();

	void Update() override;
	void Draw() override;

	// (Optional) external collision check
	bool CheckHit(int px, int py, int pw, int ph);

private:
	int   hImage;
	float x, y;
	int   width;
	int   height;

	// --- pop-up control ---
	float moveSpeed;
	float offsetY;
	bool  isActive;
	bool  isExtended;

	void Activate();

	// --- new static control ---
	Type type_ = Type::PopUp;
	Dir  dir_ = Dir::Up;

private:
	// Builds the triangle used for collision (in world coords)
	bool GetTriangle(VECTOR& t1, VECTOR& t2, VECTOR& t3) const;

	// Direction -> rotation angle (radians)
	float GetAngleRad() const;
};