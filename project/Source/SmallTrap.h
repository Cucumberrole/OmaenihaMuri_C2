#pragma once
#include "../Library/GameObject.h"

class SmallTrap : public GameObject
{
public:
	SmallTrap(int x, int y);
	~SmallTrap();
	void Update() override;
	void Draw() override;

	bool CheckHit(int px, int py, int pw, int ph);

private:
	int SImage;    // 小さい針の画像ハンドル
	float x, y;    // 座標
	int width;     // 幅（画像サイズ）
	int height;    // 高さ



};