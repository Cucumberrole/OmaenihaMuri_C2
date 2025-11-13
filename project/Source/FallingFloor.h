#pragma once
#include "../Library/GameObject.h"

class FallingFloor : public GameObject
{
public:
    FallingFloor(int sx, int sy);
    ~FallingFloor();

    void Update() override;
    void Draw() override;

private:
    int hImage;          // 床画像
    float x, y;          // 位置
    float velocityY;     // 落下速度
    bool isFalling;      // 落下中か？
    bool isLanded;       // 着地済みか？
    float gravity;       // 重力加速度

    void StartFalling(); // プレイヤー検知で落下開始
};
