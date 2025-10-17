#include "Player.h"
#include "Field.h"
static const float Gravity = 0.2;
static const float V0 = -10.0;

//�R���X�g���N�^�\
//�ŏ���1�񂾂��K���Ă΂��B
Player::Player()
{
	hImage = LoadGraph("data/image/aoi.png");
	x = 200;
	y = 500;
	velocity =0;
	onGround = false;
}
Player::Player(int sx, int sy)
{
	hImage = LoadGraph("data/image/aoi.png");
	x = sx;
	y = sy;
	velocity = 0;
	onGround = false;
}
//�f�X�g���N�^�\
//�Ō��1�񂾂��Ă΂��
Player::~Player()
{
}
//�v�Z����Ƃ���
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
	//�����ŃX�N���[��������
	Field* field = FindGameObject<Field>();
	int sc = field->GetScrollX();
	if (x-sc >= 300) {
		field->SetScrollX(x-300);

	}

}
//�\������Ƃ���
void Player::Draw()
{
	Field* field = FindGameObject<Field>();
	int sc = field->GetScrollX();
	DrawRectGraph(x, y,0,0,64,64 ,hImage, 1);

}
