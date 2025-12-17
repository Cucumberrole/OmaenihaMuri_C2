#pragma once
#include "../Library/GameObject.h"

class SmallTrap2 : public GameObject
{
public:
	SmallTrap2(int x, int y);
	~SmallTrap2();
	void Update() override;
	void Draw() override;
private:
	int SImage;    // 小さい針の画像ハンドル
	float x, y;    // 座標
	int width;     // 幅（画像サイズ）
	int height;    // 高さ
};

