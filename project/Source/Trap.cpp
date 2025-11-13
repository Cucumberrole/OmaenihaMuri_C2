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
}

Trap::~Trap()
{
    DeleteGraph(hImage);
}

void Trap::Update()
{
    // --- プレイヤーとの当たり判定 ---
    Player* player = FindGameObject<Player>();
    if (player == nullptr) return;

    // --- プレイヤー座標 ---
    float px = player->GetX();
    float py = player->GetY();
    float pw = 64.0f;
    float ph = 64.0f;

    // --- プレイヤーの矩形を4辺の線分に分解 ---
    VECTOR top1 = VGet(px, py, 0);
    VECTOR top2 = VGet(px + pw, py, 0);
    VECTOR bottom1 = VGet(px, py + ph, 0);
    VECTOR bottom2 = VGet(px + pw, py + ph, 0);
    VECTOR left1 = VGet(px, py, 0);
    VECTOR left2 = VGet(px, py + ph, 0);
    VECTOR right1 = VGet(px + pw, py, 0);
    VECTOR right2 = VGet(px + pw, py + ph, 0);

    // --- 針（三角形）の3頂点（下が基部、上がとがった先端） ---
    VECTOR tri1 = VGet(x, y + 64, 0);       // 左下
    VECTOR tri2 = VGet(x + 64, y + 64, 0);  // 右下
    VECTOR tri3 = VGet(x + 32, y, 0);       // 上頂点（針の先）

    // --- 各辺と針との交差判定 ---
    HITRESULT_LINE result1 = HitCheck_Line_Triangle(top1, top2, tri1, tri2, tri3);
    HITRESULT_LINE result2 = HitCheck_Line_Triangle(bottom1, bottom2, tri1, tri2, tri3);
    HITRESULT_LINE result3 = HitCheck_Line_Triangle(left1, left2, tri1, tri2, tri3);
    HITRESULT_LINE result4 = HitCheck_Line_Triangle(right1, right2, tri1, tri2, tri3);

    if (result1.HitFlag || result2.HitFlag || result3.HitFlag || result4.HitFlag)
    {
        player->DestroyMe();
    }
}

void Trap::Draw()
{
    DrawGraph((int)x, (int)y, hImage, TRUE);

}