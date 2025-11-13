#include "FallingFloor.h"
#include "Player.h"
#include "Field.h"
#include <DxLib.h>

FallingFloor::FallingFloor(int sx, int sy)
{
    hImage = LoadGraph("data/image/New Blo.png");
    x = static_cast<float>(sx);
    y = static_cast<float>(sy);

    velocityY = 0.0f;
    isFalling = false;
    isLanded = false;
    gravity = 0.4f;

    SetDrawOrder(90); // Field(100)より手前、Player(0)より奥
}

FallingFloor::~FallingFloor()
{
    DeleteGraph(hImage);
}

void FallingFloor::Update()
{
    if (isLanded) return; // もう止まっているなら何もしない

    Player* player = FindGameObject<Player>();
    if (player == nullptr) return;

    float px = player->GetX();
    float py = player->GetY();

    // --- プレイヤーが真下を通過したら落下開始 ---
    if (!isFalling && px + 64 > x && px < x + 64 && py > y)
    {
        StartFalling();
    }

    // --- 落下処理 ---
    if (isFalling)
    {
        velocityY += gravity;
        y += velocityY;

        // --- 下のFieldブロックに当たったら停止 ---
        Field* field = FindGameObject<Field>();
        if (field)
        {
            int push1 = field->HitCheckDown(x + 5, y + 64);
            int push2 = field->HitCheckDown(x + 59, y + 64);
            int push = max(push1, push2);

            if (push > 0)
            {
                y -= push;        // 押し戻して止める
                velocityY = 0.0f;
                isFalling = false;
                isLanded = true;  // 着地完了
            }
        }
    }
}

void FallingFloor::StartFalling()
{
    isFalling = true;
    velocityY = 0.0f;
}

void FallingFloor::Draw()
{
    DrawRectGraph(static_cast<int>(x), static_cast<int>(y), 0, 0, 64, 64, hImage, TRUE);
}
