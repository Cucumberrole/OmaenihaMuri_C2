#pragma once
#include "../Library/GameObject.h"

class Boss : public GameObject
{
public:
	Boss(int sx, int sy);
	~Boss();

	void Update() override;
	void Draw() override;

private:
	int hImage;

	float x, y;
	float vx, vy;
	float gravity;

	int hp;

	enum State {
		STATE_IDLE,
		STATE_WALK,
		STATE_CHASE,
		STATE_JUMP_ATTACK,
		STATE_COOLDOWN
	};
	State state;

	int stateTimer;
	bool facingLeft; // trueÅFç∂å¸Ç´

	void ChangeState(State newState);
	void DoIdle();
	void DoWalk();
	void DoChase();
	void DoJumpAttack();
	void DoCooldown();
	void CollisionWithMap();
	void AttackCheck();
};