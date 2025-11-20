#pragma once
#include "../Library/GameObject.h"


class Player : public GameObject
{
public:
    Player();                 // デフォルトコンストラクタ
    Player(int sx, int sy);   // 座標指定付きコンストラクタ
    ~Player();                // デストラクタ

    void Update() override;   // 状態更新（入力処理・物理計算など）
    void Draw() override;     // 描画処理

    float GetX() const;
    float GetY() const;

    float GetRadius() const { return CHARACTER_WIDTH * 0.35f; }
private:
    // --- 基本パラメータ ---
    int hImage;         // プレイヤー画像ハンドル
    float x, y;         // プレイヤー座標（左上基準）
    float velocity;     // 垂直方向の速度（重力・ジャンプに使用）
    bool onGround;      // 地面に接地しているかどうかのフラグ
    int jumpcount;      // 残りジャンプ回数（二段ジャンプ制御用）
    int Maxjumpcount;   // 最大ジャンプ回数
    int hp;             // 残機

    // --- アニメーション設定 ---
    static const int CHARACTER_WIDTH = 64;     // 1コマの横幅（ピクセル）
    static const int CHARACTER_HEIGHT = 64;    // 1コマの縦幅（ピクセル）
    static const int ATLAS_WIDTH = 12;         // スプライトシートの横方向のコマ数
    static const int ATLAS_HEIGHT = 1;         // スプライトシートの縦方向のコマ数
    static const int ANIM_FRAME_COUNT = 12;    // 使用するアニメーションコマ数
    static const int ANIM_FRAME_INTERVAL = 5;  // コマを切り替えるフレーム間隔
    static const int WALK_SPEED = 3;           // プレイヤーの横移動速度（ドット/フレーム）

    // --- アニメーション関連変数 ---
    int animIndex;      // 現在のアニメーションフレーム番号
    int animFrame;      // フレームカウンタ（アニメ速度制御）
    bool direction;     // 向き（true=左向き / false=右向き）
};