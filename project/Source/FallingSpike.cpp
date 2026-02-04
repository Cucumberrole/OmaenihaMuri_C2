#include "Collision.h"
#include "FallingSpike.h"
#include "Field.h"
#include "Player.h"
#include "SoundCache.h"
#include <algorithm>
#include <DxLib.h>
#include <vector>

std::vector<FallingSpike*> FallingSpike::s_allSpikes;

//--------------------------------------
// コンストラクタ
//--------------------------------------
FallingSpike::FallingSpike(int sx, int sy, bool chaseAfterLand, int triggerGroupId)
{
	hImage = LoadGraph("data/image/hariBottom.png");

	x = static_cast<float>(sx);
	y = static_cast<float>(sy);

	vy = 0.0f;
	gravity = 0.8f;
	landed = false;

	int w, h;
	GetGraphSize(hImage, &w, &h);
	width = w;
	height = h;

	// トリガー関連
	this->triggerGroupId = triggerGroupId;
	// groupId = 0 → 最初から動く / 0以外 → トリガーが来るまで停止
	active = (triggerGroupId == 0);

	// 追尾関連
	isChaser = chaseAfterLand;
	startedChase = false;
	vx = 0.0f;
	chaseSpeed = 6.0f;  // 横に走る速さ

	// 登録
	s_allSpikes.push_back(this);

	const char* messages[] = {
	"動くよ～ん",
	"死んじゃった？",
	};

	int count = sizeof(messages) / sizeof(messages[0]);
	tauntText = messages[GetRand(count - 1)];

	showTaunt = false; // まだ表示しない

	SpikeSE = SoundCache::Get("data/BGM/spikeDeath.mp3");
}

//--------------------------------------
// デストラクタ
//--------------------------------------
FallingSpike::~FallingSpike()
{
	auto it = std::find(s_allSpikes.begin(), s_allSpikes.end(), this);
	if (it != s_allSpikes.end())
	{
		s_allSpikes.erase(it);
	}

	DeleteGraph(hImage);
	StopSoundMem(SpikeSE);
}

//--------------------------------------
// Update
//--------------------------------------
void FallingSpike::Update()
{
	if (!active) { return; } // トリガーが起動されるまで完全停止

	Field* field = FindGameObject<Field>();



	//----------------------------------
	// まだ落下中なら重力処理
	//----------------------------------
	if (!landed)
	{
		vy += gravity;
		y += vy;

		if (field)
		{
			int tx = static_cast<int>((x + width / 2) / 64); // 中心X
			int ty = static_cast<int>((y + height) / 64); // 足元Y

			if (field->IsBlock(tx, ty))
			{
				// ブロックの上に乗せる
				y = ty * 64.0f - height;
				vy = 0.0f;
				landed = true;
			}
		}
	}
	else
	{
		//----------------------------------
		// 着地済み：追尾モードなら横移動
		//----------------------------------
		if (isChaser)
		{
			Player* player = FindGameObject<Player>();

			if (!startedChase && player)
			{
				float pxCenter = player->GetX() + 32.0f;
				float sxCenter = x + width / 2.0f;

				if (pxCenter < sxCenter)
					vx = -chaseSpeed;   // 左へ
				else
					vx = chaseSpeed;   // 右へ

				startedChase = true;

				showTaunt = true;
			}

			// 横に平行移動
			if (startedChase)
			{
				float oldX = x;
				x += vx;

				// 壁にぶつかったら止まる
				if (field)
				{
					float checkX = (vx > 0.0f) ? (x + width) : x;
					float checkY = y + height / 2.0f;

					int tileX = static_cast<int>(checkX / 64);
					int tileY = static_cast<int>(checkY / 64);

					if (field->IsBlock(tileX, tileY))
					{
						x = oldX;
						vx = 0.0f;
						isChaser = false;
						DestroyMe();
					}
				}
			}
		}
	}

	//----------------------------------
	// プレイヤーとの当たり判定（落下中・移動中どちらでも有効）
	//----------------------------------
	Player* player = FindGameObject<Player>();
	if (player)
	{
		float px = player->GetX();
		float py = player->GetY();

		// プレイヤー円
		VECTOR center = VGet(px + 32.0f, py + 32.0f, 0.0f);
		float  radius = player->GetRadius();

		// 針の三角形（下向き）
		VECTOR t1 = VGet(x, y, 0.0f); // 左上
		VECTOR t2 = VGet(x + width, y, 0.0f); // 右上
		VECTOR t3 = VGet(x + width / 2, y + height, 0.0f); // 下の先端

		if (HitCheck_Circle_Triangle(center, radius, t1, t2, t3))
		{
			PlaySoundMem(SpikeSE, DX_PLAYTYPE_BACK);
			player->ForceDie();
			player->SetDead();
		}
	}
}

//--------------------------------------
// Draw
//--------------------------------------
void FallingSpike::Draw()
{
	DrawGraph(static_cast<int>(x), static_cast<int>(y), hImage, TRUE);

	if (showTaunt)
	{
		DrawString(
			(int)(x + width + 4), // 右側
			(int)(y - 20),        // 少し上
			tauntText,
			GetColor(255, 255, 255)
		);
	}

#ifdef _DEBUG
	// 当たり判定三角形の表示（デバッグ用）
	//int tx1 = static_cast<int>(x);
	//int ty1 = static_cast<int>(y);

	//int tx2 = static_cast<int>(x + width);
	//int ty2 = static_cast<int>(y);

	//int tx3 = static_cast<int>(x + width / 2);
	//int ty3 = static_cast<int>(y + height);

	//int col = GetColor(0, 255, 0);
	//DrawTriangle(tx1, ty1, tx2, ty2, tx3, ty3, col, FALSE);
#endif
}

// 起動用
void FallingSpike::Activate()
{
	active = true;
}

void FallingSpike::ActivateGroup(int groupId)
{
	for (FallingSpike* spike : s_allSpikes)
	{
		if (!spike) continue;
		if (spike->triggerGroupId == groupId)
		{
			spike->Activate();
		}
	}
}
