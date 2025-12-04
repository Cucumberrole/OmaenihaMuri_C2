#include "Telop.h"
#include "Player.h"
#include "Field.h"

Telop::Telop():isVisible(false)
{
}

Telop::~Telop()
{
}

void Telop::Update()
{
    // 例: スペースキーが押されたら非表示にする（リトライを想定）
    // if (isVisible && IsSpaceKeyJustPressed()) { // IsSpaceKeyJustPressedは仮の関数
    //     Deactivate();
    //     // ゲームのリセット処理を呼び出す
    // }
}

void Telop::Draw()
{
    if (isVisible) {
        DrawString(620, 140, messageText.c_str(), GetColor(0, 0, 0));
    }

}

void Telop::Activate(const std::string& message)
{
    messageText = message;
    isVisible = true;
}

void Telop::Deactivate()
{
    isVisible = false;
}
