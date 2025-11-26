#pragma once
#include "../Library/GameObject.h"

class SmallTrap : public GameObject 
{
public:
	SmallTrap();
	~SmallTrap();
	void Update() override;
	void Draw() override;

private:
	int hImage;    // 針の画像ハンドル
	float x, y;    // 座標
	int width;     // 幅（画像サイズ）
	int height;    // 高さ

};