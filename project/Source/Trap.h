#pragma once
#include "../Library/GameObject.h"

class Trap : public GameObject {
    public:
        Trap(int x, int y);
        ~Trap();
        void Update() override;
        void Draw() override;

        // --- 衝突判定用 ---
        bool CheckHit(int px, int py, int pw, int ph); // プレイヤー矩形と当たっているか？

    private:
        int hImage;   // 針の画像ハンドル
        int x, y;     // 座標
        int width;    // 幅（画像サイズ）
        int height;   // 高さ
};