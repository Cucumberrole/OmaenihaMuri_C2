#include "Player.h"
#include "Field.h"
#include "FallingFloor.h"
#include "../Library/Trigger.h"
#include <assert.h>


// --- 定数 ---
static const float Gravity = 0.2f;  // 重力加速度
static const float V0 = -5.0f;     // ジャンプ初速度（上方向）

//--------------------------------------
// デフォルトコンストラクタ
//--------------------------------------
Player::Player()
{
    hImage = LoadGraph("data/image/おまえ歩き.png");
    assert(hImage != -1);

    x = 200;
    y = 500;
    velocity = 0;
    onGround = false;

    jumpcount = 0;
    Maxjumpcount = 1;

    // アニメーション初期化
    animIndex = 0;
    animFrame = 0;
    direction = true;
}

//--------------------------------------
// 座標指定コンストラクタ
//--------------------------------------
Player::Player(int sx, int sy)
{
    hImage = LoadGraph("data/image/おまえ歩き.png");
    assert(hImage != -1);

    x = static_cast<float>(sx);
    y = static_cast<float>(sy);
    velocity = 0;
    onGround = false;

    jumpcount = 0;
    Maxjumpcount = 1;

    // アニメーション初期化
    animIndex = 0;
    animFrame = 0;
    direction = false;

	SetDrawOrder(0); // 描画順を変更
}

//--------------------------------------
// デストラクタ
//--------------------------------------
Player::~Player()
{
    DeleteGraph(hImage);
}

//--------------------------------------
// 座標取得用
//--------------------------------------
float Player::GetX() const
{
    return x;
}

float Player::GetY() const
{
    return y;
}

//--------------------------------------
// Update()
//--------------------------------------
void Player::Update()
{
    // --- 地面にいるときはジャンプ回数をリセット ---
    if (onGround && jumpcount < Maxjumpcount) {
        jumpcount += 1;
    }

    int moveX = 0;
    //--------------------------------------
    // 右移動（Dキー）
    //--------------------------------------
    if (CheckHitKey(KEY_INPUT_D)) {
        moveX = WALK_SPEED;
        direction = false;

        // --- Field 判定 ---
        Field* field = FindGameObject<Field>();
        int push1 = field->HitCheckRight(x + 60, y + 5);
        int push2 = field->HitCheckRight(x + 60, y + 63);
        int push = max(push1, push2);

        // --- 落下床 判定（複数床対応） ---
        auto floors = FindGameObjects<FallingFloor>();
        for (auto f : floors) {
            int p1 = f->HitCheckRight(x + 60, y + 5);
            int p2 = f->HitCheckRight(x + 60, y + 63);
            push = max(push, max(p1, p2));
        }

        x -= push;
    }

    //--------------------------------------
    // 左移動（Aキー）
    //--------------------------------------
    if (CheckHitKey(KEY_INPUT_A)) {
        moveX = -WALK_SPEED;
        direction = true;

        Field* field = FindGameObject<Field>();
        int push1 = field->HitCheckLeft(x + 4, y + 5);
        int push2 = field->HitCheckLeft(x + 4, y + 63);
        int push = max(push1, push2);

        auto floors = FindGameObjects<FallingFloor>();
        for (auto f : floors) {
            int p1 = f->HitCheckLeft(x + 4, y + 5);
            int p2 = f->HitCheckLeft(x + 4, y + 63);
            push = max(push, max(p1, p2));
        }

        x += push;
    }

    //--------------------------------------
    // 歩行アニメーション更新
    //--------------------------------------
    if (moveX != 0) {
        // 一定間隔で次のコマに切り替える
        animFrame = (animFrame + 1) % ANIM_FRAME_INTERVAL;
        if (animFrame == 0) {
            animIndex = (animIndex + 1) % ANIM_FRAME_COUNT;
        }

        // 実際の位置を更新
        x += moveX;
    }

	//マップクラスの取得
	Field* field = FindGameObject<Field>(); 

    //--------------------------------------
    // ジャンプ処理（接地中）
    //--------------------------------------
    if (onGround) {
        if (KeyTrigger::CheckTrigger(KEY_INPUT_SPACE)) {
            velocity = V0;     // 上方向に加速
            onGround = false;  // 空中へ
        }
    }

    //--------------------------------------
    // 二段ジャンプ処理
    //--------------------------------------
    if (!onGround && jumpcount == Maxjumpcount) {
        if (KeyTrigger::CheckTrigger(KEY_INPUT_SPACE)) {
            jumpcount -= 1;
            velocity = V0;
        }
    }

    //--------------------------------------
    // 重力適用
    //--------------------------------------
    y += velocity;
    velocity += Gravity;

    //--------------------------------------
    // 当たり判定（下方向：床）
    //--------------------------------------

    if (velocity >= 0) { // 落下中
        int push1 = field->HitCheckDown(x + 14, y + 64);
        int push2 = field->HitCheckDown(x + 50, y + 64);
        int push = max(push1, push2);

        // --- 落下床との当たり判定 ---
        auto floors = FindGameObjects<FallingFloor>();
        for (auto f : floors) {
            int p1 = f->HitCheckDown(x + 14, y + 64);
            int p2 = f->HitCheckDown(x + 50, y + 64);
            push = max(push, max(p1, p2));
        }

        if (push > 0) {
            y -= push - 1;
            velocity = 0;
            onGround = true;
        }
        else {
            onGround = false;
        }
    }
    else { // 上昇中
        int push1 = field->HitCheckUp(x + 14, y + 5);
        int push2 = field->HitCheckUp(x + 50, y + 5);
        int push = max(push1, push2);

        // --- 落下床との当たり判定 ---
        auto floors = FindGameObjects<FallingFloor>();
        for (auto f : floors) {
            int p1 = f->HitCheckUp(x + 14, y + 5);
            int p2 = f->HitCheckUp(x + 50, y + 5);
            push = max(push, max(p1, p2));
        }

        if (push > 0) {
            y += push;
            velocity = 0;
        }
    }

}

//--------------------------------------
// Draw()
//--------------------------------------
void Player::Draw()
{
    Field* field = FindGameObject<Field>();

    //--------------------------------------
    // 現在のスプライトシート上での位置を算出
    //--------------------------------------
    int xRect = (animIndex % ATLAS_WIDTH) * CHARACTER_WIDTH;
    int yRect = (animIndex / ATLAS_WIDTH) * CHARACTER_HEIGHT;

    //--------------------------------------
    // キャラクター描画
    // DrawRectGraph(x, y, 切り出しX, 切り出しY, 幅, 高さ, 画像, 透過, 左右反転)
    //--------------------------------------
    DrawRectGraph(
        static_cast<int>(x),
        static_cast<int>(y),
        xRect,
        yRect,
        CHARACTER_WIDTH,
        CHARACTER_HEIGHT,
        hImage,
        TRUE,
        direction
    );

    //--------------------------------------
    // デバッグ用座標表示
    //--------------------------------------
    DrawFormatString(0, 100, GetColor(255, 255, 255), "x: %.2f", x);
    DrawFormatString(0, 120, GetColor(255, 255, 255), "y: %.2f", y);
}