#pragma once
#include "../Library/GameObject.h"

class EnemyChaser : public GameObject
{
public:
	EnemyChaser(float sx, float sy);
	~EnemyChaser();

	void Update() override;
	void Draw() override;

private:


    static const int CHARACTER_WIDTH = 1280;
    static const int CHARACTER_HEIGHT = 640;
    static const int ATLAS_WIDTH = 2;
    static const int ATLAS_HEIGHT = 1;
    static const int ANIM_FRAME_COUNT = 2;     //  アニメ枚数
    static const int ANIM_FRAME_INTERVAL = 4;   //  アニメーション間隔(frame)
    static const int WALK_SPEED = 4;            //  移動速度(dot/frame)

    int animImage;      //  アニメーション用画像ハンドル
    int animIndex;      //  どこのコマを表示するか
    int animFrame;      //  アニメーション間隔
    int xPosition;      //  キャラクターのX座標
    bool direction;     //  移動方向(true:右方向 / false:左方向)

	float x, y;
	float speed;   // 移動速度
	float size;    // 当たり判定サイズ
};