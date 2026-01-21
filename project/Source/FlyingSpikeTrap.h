#pragma once
#include "../Library/GameObject.h"
#include <DxLib.h>
#include <vector>
#include <cmath>

#include "Player.h"
#include "Field.h"
#include "Collision.h" // HitCheck_Circle_Triangle ���g��

// �� SpikeDir �ƏՓ˂��Ȃ����O
enum class TrapDir
{
	Left,
	Right,
	Up,
	Down
};

// �g���K�[ + ���� + �j�̔��� + �����蔻�� + �`�� ��S���܂Ƃ߂��N���X
// �d�l�F�v���C���[���͈͂Ɂu�������u�ԁv1�񂾂����ˁB�Ȍ�͓�x�Ɣ��˂��Ȃ��B
class FlyingSpikeTrap : public GameObject
{
public:
	// rect�F�g���K�[�͈́i���[���h���W�̋�`�j
	// fromDir�F�ǂ����i��ʊO�j����o�����邩�iRight�Ȃ�E��ʊO�����֔�ԁj
	// speed�F���x�i���̃g���b�v�ŗL�j
	// laneWorld�FNAN�Ȃ�v���C���[�ʒu�ɍ��킹��B�l����Ȃ烌�[���Œ�i���E���˂Ȃ�Y�Œ�A�㉺���˂Ȃ�X�Œ�j
	// spawnMargin�F��ʊO�X�|�[���̗]��
	FlyingSpikeTrap(
		float rectX, float rectY, float rectW, float rectH,
		TrapDir fromDir,
		float speed,
		float laneWorld = NAN,
		float spawnMargin = 64.0f,
		int startOffsetBlocks = 0
	);

	~FlyingSpikeTrap();

	void Update() override;
	void Draw() override;

private:
	struct SpikeShot
	{
		float x, y;      // ����
		float vx, vy;    // ���x
		TrapDir dir;     // �i�s����
		float speed;     // ���x�̑傫��
		bool alive;
	};

private:
	// --- �g���K�[ ---
	bool IsPlayerInside(Player* player) const;

	// --- ���� ---
	void Fire(Player* player);
	void CalcSpawnParams(
		const VECTOR& targetCenter,
		float spikeW, float spikeH,
		float& outSpawnX, float& outSpawnY,
		TrapDir& outFlyDir
	) const;

	// --- �e�i�j�j�X�V ---
	void UpdateShots(Player* player, Field* field);
	void BuildTriangleVertices(const SpikeShot& s, VECTOR& t1, VECTOR& t2, VECTOR& t3) const;

	// --- �摜���L ---
	void EnsureSharedImageLoaded();
	static int sImage;
	static int sRefCount;

private:
	// �g���K�[�͈�
	float x, y, w, h;

	// ���ːݒ�
	TrapDir fromDir;
	float speed;
	float laneWorld;
	float spawnMargin;

	// 起動時の開始位置オフセット（タイル数）。0=従来（画面外から）。1=1ブロック(64px)…
	int startOffsetBlocks;

	// ������ԁi1�񔭓��̂݁j
	bool wasInside;
	bool triggered;

	// �e�Ǘ�
	std::vector<SpikeShot> shots;

	// �摜�T�C�Y�ihariLeft.png ����]���ĕ`��j
	float spikeW;
	float spikeH;
};
