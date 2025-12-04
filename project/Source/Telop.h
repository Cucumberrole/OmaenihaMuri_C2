#pragma once
#include "../Library/GameObject.h"

class Telop
{
public:
	Telop();
	~Telop();
	void Update();
	void Draw();
	void Activate(const std::string& message);// 外部からテロップ表示を指示する関数
	void Deactivate();                        // リトライなどで非表示にする関数
private:
	std::string messageText; // 表示する文字列
	bool isVisible;          // 表示フラグ
};

