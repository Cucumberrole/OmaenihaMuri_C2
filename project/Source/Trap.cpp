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

bool HitCheck_Circle_Line(VECTOR center, float radius, VECTOR a, VECTOR b)
{
    VECTOR ab = VSub(b, a);
    VECTOR ac = VSub(center, a);

    float abLen2 = ab.x * ab.x + ab.y * ab.y;

    float t = (ab.x * ac.x + ab.y * ac.y) / abLen2;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    VECTOR closest = VAdd(a, VScale(ab, t));

    float dx = center.x - closest.x;
    float dy = center.y - closest.y;

    return (dx * dx + dy * dy <= radius * radius);
}

bool PointInTriangle(VECTOR p, VECTOR a, VECTOR b, VECTOR c)
{
    VECTOR v0 = VSub(c, a);
    VECTOR v1 = VSub(b, a);
    VECTOR v2 = VSub(p, a);

    float dot00 = VDot(v0, v0);
    float dot01 = VDot(v0, v1);
    float dot02 = VDot(v0, v2);
    float dot11 = VDot(v1, v1);
    float dot12 = VDot(v1, v2);

    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    return (u >= 0) && (v >= 0) && (u + v < 1);
}

bool HitCheck_Circle_Triangle(VECTOR center, float radius,
    VECTOR t1, VECTOR t2, VECTOR t3)
{
    if (HitCheck_Circle_Line(center, radius, t1, t2)) return true;
    if (HitCheck_Circle_Line(center, radius, t2, t3)) return true;
    if (HitCheck_Circle_Line(center, radius, t3, t1)) return true;

    // 円の中心が三角形内部にある場合
    if (PointInTriangle(center, t1, t2, t3)) return true;

    return false;
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
        float trapTop = y - offsetY;
        float trapBottom = trapTop + height;

        if (px + pw > x && px < x + width &&
            py + ph > trapTop && py + ph < trapBottom)
        {
            Activate();
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
        float baseY = y - offsetY;

        VECTOR tri1 = VGet(x, baseY + height, 0);          // 左下
        VECTOR tri2 = VGet(x + width, baseY + height, 0);  // 右下
        VECTOR tri3 = VGet(x + width / 2, baseY, 0);       // 上の先端

        VECTOR center = VGet(px + pw / 2, py + ph / 2, 0);
        float radius = pw * 0.35f;

        if (HitCheck_Circle_Triangle(center, radius, tri1, tri2, tri3))
        {
            player->DestroyMe();
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