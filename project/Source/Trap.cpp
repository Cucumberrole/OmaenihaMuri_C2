#include <DxLib.h>
#include "Trap.h"
#include "Player.h"

Trap::Trap(int sx, int sy)
{
    hImage = LoadGraph("data/image/hari.png"); // 針画像
    x = static_cast<float>(sx);
    y = static_cast<float>(sy);
    width = 64;
    height = 64;

    offsetY = 0.0f;
    isActive = false;
    isExtended = false;
    moveSpeed = 10.0f; // 上にせり出すスピード

	SetDrawOrder(100); // 描画順序
}

Trap::~Trap()
{
    DeleteGraph(hImage);
}

void Trap::Update()
{
    Player* player = FindGameObject<Player>();
    if (player == nullptr) return;

    float px = player->GetX();
    float py = player->GetY();
    float pw = 64.0f;
    float ph = 64.0f;

    // --- プレイヤーが上のブロックを踏んだら発動 ---
    if (!isActive && !isExtended)
    {
        if (px + pw > x && px < x + width &&
            py + ph > y - height && py + ph < y)
        {
            Activate(); // 発動開始
        }
    }

    // --- トラップ上昇アニメーション ---
    if (isActive && !isExtended)
    {
        offsetY += moveSpeed;
        if (offsetY >= 64.0f)
        {
            offsetY = 64.0f;
            isActive = false;
            isExtended = true; // 出きった
        }
    }

    // --- 当たり判定（出ている間だけ） ---
    if (isExtended)
    {
        // 針（三角形の頂点）
        VECTOR tri1 = VGet(x, y + height - offsetY, 0);       // 左下
        VECTOR tri2 = VGet(x + width, y + height - offsetY, 0); // 右下
        VECTOR tri3 = VGet(x + width / 2, y - offsetY, 0);    // 上の先端

        // プレイヤーの矩形を4辺に分解
        VECTOR top1 = VGet(px, py, 0);
        VECTOR top2 = VGet(px + pw, py, 0);
        VECTOR bottom1 = VGet(px, py + ph, 0);
        VECTOR bottom2 = VGet(px + pw, py + ph, 0);
        VECTOR left1 = VGet(px, py, 0);
        VECTOR left2 = VGet(px, py + ph, 0);
        VECTOR right1 = VGet(px + pw, py, 0);
        VECTOR right2 = VGet(px + pw, py + ph, 0);

        // 各辺との交差判定
        HITRESULT_LINE r1 = HitCheck_Line_Triangle(top1, top2, tri1, tri2, tri3);
        HITRESULT_LINE r2 = HitCheck_Line_Triangle(bottom1, bottom2, tri1, tri2, tri3);
        HITRESULT_LINE r3 = HitCheck_Line_Triangle(left1, left2, tri1, tri2, tri3);
        HITRESULT_LINE r4 = HitCheck_Line_Triangle(right1, right2, tri1, tri2, tri3);

        if (r1.HitFlag || r2.HitFlag || r3.HitFlag || r4.HitFlag)
        {
            player->DestroyMe();
            SceneManager::ChangeScene("TITLE");
        }
    }
}

void Trap::Activate()
{
    isActive = true;
}

void Trap::Draw()
{
    // offsetY分だけ上に描画
    DrawRectGraph(
        static_cast<int>(x),
        static_cast<int>(y - offsetY),
        0, 0, width, height,
        hImage,
        TRUE
    );
}

bool Trap::CheckHit(int px, int py, int pw, int ph)
{
    return (px + pw > x && px < x + width &&
        py + ph > y - offsetY && py < y + height);
}