#include "Player.h"
#include "Field.h"
static const float Gravity = 0.2;
static const float V0 = -10.0;

//コンストラクタ―
//最初に1回だけ必ず呼ばれる。
Player::Player()
{
	hImage = LoadGraph("data/image/おまえ歩き.png");
	x = 200;
	y = 500;
	velocity =0;
	onGround = false;
}
Player::Player(int sx, int sy)
{
	hImage = LoadGraph("data/image/おまえ歩き.png");
	x = sx;
	y = sy;
	velocity = 0;
	onGround = false;
}
//デストラクタ―
//最後に1回だけ呼ばれる
Player::~Player()
{
}
//計算するところ
void Player::Update()
{
	if (CheckHitKey(KEY_INPUT_D)) {
		x += 2;
		Field* field = FindGameObject<Field>();
		if (y >= 400) {
			int push2 = field->HitCheckRight(x + 50, y + 63);
			x -= push2;
		}
	}
	if (CheckHitKey(KEY_INPUT_A)) {
		x -= 2;
		Field* field = FindGameObject<Field>();
		if (y >= 400) {
			int push1 = field->HitCheckLeft(x + 14, y + 5);

			x += push1;
		}
	}

	
	if (onGround == true) {
		if (CheckHitKey(KEY_INPUT_SPACE)) {
			velocity = V0;
		}

	}
	y += velocity;
	velocity += Gravity;
	if (velocity >= 0) {
		Field* field = FindGameObject<Field>();
		
				int push1 = field->HitCheckDown(x + 14, y + 64);
				int push2 = field->HitCheckDown(x + 50, y + 64);
				int push = max(push1, push2);
				if (push > 0) {
					y -= push - 1;
					velocity = 0;
					onGround = true;
				}
				else {
					onGround = false;
				}
			
		
	}
	else {
		Field* field = FindGameObject<Field>();
			int push1 = field->HitCheckUp(x + 14, y + 5);
			int push2 = field->HitCheckUp(x + 50, y + 5);
			int push = max(push1, push2);
			if (push > 0) {
				y += push;
				velocity = 0;
			}
		
	}
	//ここでスクロールを書く
	Field* field = FindGameObject<Field>();
	int sc = field->GetScrollX();
	if (x-sc >= 300) {
		field->SetScrollX(x-300);

	}

}
//表示するところ
void Player::Draw()
{
	Field* field = FindGameObject<Field>();
	int sc = field->GetScrollX();
	//DrawRectGraph(x, y,0,0,64,64 ,hImage, 1);
	DrawRectGraph(0, 0, 0, 0, 64, 64, hImage, 1);
	DrawFormatString(0, 100, GetColor(255, 255, 255), "x::%4f", x);
	DrawFormatString(0, 120, GetColor(255, 255, 255), "y::%4f", y);

}
