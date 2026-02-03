#include "FlyingSpikeTrap.h"
#include "SoundCache.h"
#include <algorithm>

int FlyingSpikeTrap::sImage = -1;
int FlyingSpikeTrap::sRefCount = 0;

namespace
{
	constexpr float kPi = 3.14159265358979323846f;

	static float AngleFromDir(TrapDir d)
	{
		// 基本画像が「左向き(hariLeft.png)」想定
		switch (d)
		{
		case TrapDir::Left:  return 0.0f;
		case TrapDir::Right: return kPi;
		case TrapDir::Up:    return -kPi * 0.5f;
		case TrapDir::Down:  return  kPi * 0.5f;
		default:             return 0.0f;
		}
	}

	static void VelocityFromDir(TrapDir d, float speed, float& outVx, float& outVy)
	{
		switch (d)
		{
		case TrapDir::Left:  outVx = -speed; outVy = 0.0f;  break;
		case TrapDir::Right: outVx = speed; outVy = 0.0f;  break;
		case TrapDir::Up:    outVx = 0.0f;   outVy = -speed; break;
		case TrapDir::Down:  outVx = 0.0f;   outVy = speed; break;
		default:             outVx = 0.0f;   outVy = 0.0f;  break;
		}
	}

	static TrapDir OppositeDir(TrapDir d)
	{
		switch (d)
		{
		case TrapDir::Left:  return TrapDir::Right;
		case TrapDir::Right: return TrapDir::Left;
		case TrapDir::Up:    return TrapDir::Down;
		case TrapDir::Down:  return TrapDir::Up;
		default:             return TrapDir::Left;
		}
	}
}

FlyingSpikeTrap::FlyingSpikeTrap(
	float rectX, float rectY, float rectW, float rectH,
	TrapDir fromDir,
	float speed,
	float laneWorld,
	float spawnMargin,
	int startOffsetBlocks
)
{
	x = rectX; y = rectY; w = rectW; h = rectH;

	this->fromDir = fromDir;
	this->speed = speed;
	this->laneWorld = laneWorld;
	this->spawnMargin = spawnMargin;

	this->startOffsetBlocks = startOffsetBlocks;
	wasInside = false;
	triggered = false;

	spikeW = 64.0f;
	spikeH = 64.0f;

	EnsureSharedImageLoaded();

	SpikeSE = SoundCache::Get("data/BGM/spikeDeath.mp3");
}

FlyingSpikeTrap::~FlyingSpikeTrap()
{
	sRefCount--;
	if (sRefCount <= 0 && sImage != -1)
	{
		DeleteGraph(sImage);
		sImage = -1;
		sRefCount = 0;
	}
	StopSoundMem(SpikeSE);
}

void FlyingSpikeTrap::EnsureSharedImageLoaded()
{
	if (sImage == -1)
	{
		sImage = LoadGraph("data/image/hariLeft.png");
	}

	// 毎回サイズ取得してメンバーに反映（インスタンスごとの初期値64を確実に更新）
	if (sImage != -1)
	{
		int w, h;
		GetGraphSize(sImage, &w, &h);
		if (w > 0) spikeW = (float)w;
		if (h > 0) spikeH = (float)h;
	}

	sRefCount++;
}

bool FlyingSpikeTrap::IsPlayerInside(Player* player) const
{
	if (!player) return false;

	// 既存コードに合わせてプレイヤー 64x64 前提
	const float pw = 64.0f;
	const float ph = 64.0f;

	float px = player->GetX();
	float py = player->GetY();

	return (px < x + w) && (px + pw > x) &&
		(py < y + h) && (py + ph > y);
}

void FlyingSpikeTrap::CalcSpawnParams(
	const VECTOR& targetCenter,
	float spikeW, float spikeH,
	float& outSpawnX, float& outSpawnY,
	TrapDir& outFlyDir
) const
{
	const bool hasLane = !std::isnan(laneWorld);

	// flyDir は「fromDir の逆」
	TrapDir flyDir = TrapDir::Left;
	switch (fromDir)
	{
	case TrapDir::Left:  flyDir = TrapDir::Right; break;
	case TrapDir::Right: flyDir = TrapDir::Left;  break;
	case TrapDir::Up:    flyDir = TrapDir::Down;  break;
	case TrapDir::Down:  flyDir = TrapDir::Up;    break;
	default: break;
	}

	float spawnCx = 0.0f;
	float spawnCy = 0.0f;

	// 起動時の開始位置指定（タイル 64x64 前提）
	// startOffsetBlocks > 0 のとき：トラップ中心から「指定ブロック数」だけ fromDir 方向にずらした位置から発射
	// startOffsetBlocks == 0 のとき：従来通り「画面外（プレイヤー中心基準）」から発射
	constexpr float kTileSize = 64.0f;

	if (startOffsetBlocks > 0)
	{
		const float rectCx = x + w * 0.5f;
		const float rectCy = y + h * 0.5f;
		const float offset = static_cast<float>(startOffsetBlocks) * kTileSize;

		switch (fromDir)
		{
		case TrapDir::Left:
			spawnCx = rectCx - offset;
			spawnCy = hasLane ? laneWorld : targetCenter.y;
			break;

		case TrapDir::Right:
			spawnCx = rectCx + offset;
			spawnCy = hasLane ? laneWorld : targetCenter.y;
			break;

		case TrapDir::Up:
			spawnCx = hasLane ? laneWorld : targetCenter.x;
			spawnCy = rectCy - offset;
			break;

		case TrapDir::Down:
			spawnCx = hasLane ? laneWorld : targetCenter.x;
			spawnCy = rectCy + offset;
			break;

		default:
			spawnCx = rectCx;
			spawnCy = rectCy;
			break;
		}
	}
	else
	{
		// --- 従来の挙動：プレイヤー中心から見た画面外にスポーン ---
		int sw = 0, sh = 0;
		GetDrawScreenSize(&sw, &sh);
		if (sw <= 0) sw = 1280;
		if (sh <= 0) sh = 720;

		const float halfW = sw * 0.5f;
		const float halfH = sh * 0.5f;

		switch (fromDir)
		{
		case TrapDir::Left:
			spawnCx = targetCenter.x - halfW - spawnMargin - spikeW;
			spawnCy = hasLane ? laneWorld : targetCenter.y;
			break;

		case TrapDir::Right:
			spawnCx = targetCenter.x + halfW + spawnMargin + spikeW;
			spawnCy = hasLane ? laneWorld : targetCenter.y;
			break;

		case TrapDir::Up:
			spawnCx = hasLane ? laneWorld : targetCenter.x;
			spawnCy = targetCenter.y - halfH - spawnMargin - spikeH;
			break;

		case TrapDir::Down:
			spawnCx = hasLane ? laneWorld : targetCenter.x;
			spawnCy = targetCenter.y + halfH + spawnMargin + spikeH;
			break;

		default:
			break;
		}
	}

	outFlyDir = flyDir;

	// Shotは左上座標で持つ
	outSpawnX = spawnCx - spikeW * 0.5f;
	outSpawnY = spawnCy - spikeH * 0.5f;
}

void FlyingSpikeTrap::Fire(Player* player)
{
	if (!player) return;

	// プレイヤー中心を狙う
	VECTOR targetCenter = VGet(player->GetX() + 32.0f, player->GetY() + 32.0f, 0.0f);

	float sx = 0.0f, sy = 0.0f;
	TrapDir flyDir = TrapDir::Left;
	CalcSpawnParams(targetCenter, spikeW, spikeH, sx, sy, flyDir);

	SpikeShot s{};
	s.x = sx;
	s.y = sy;
	s.dir = flyDir;
	s.speed = speed;
	s.alive = true;
	VelocityFromDir(flyDir, speed, s.vx, s.vy);

	shots.push_back(s);
}

void FlyingSpikeTrap::BuildTriangleVertices(const SpikeShot& s, VECTOR& t1, VECTOR& t2, VECTOR& t3) const
{
	float sx = s.x;
	float sy = s.y;
	float w = spikeW;
	float h = spikeH;

	switch (s.dir)
	{
	case TrapDir::Left:
		t1 = VGet(sx + w, sy, 0.0f);
		t2 = VGet(sx + w, sy + h, 0.0f);
		t3 = VGet(sx, sy + h * 0.5f, 0.0f);
		break;

	case TrapDir::Right:
		t1 = VGet(sx, sy, 0.0f);
		t2 = VGet(sx, sy + h, 0.0f);
		t3 = VGet(sx + w, sy + h * 0.5f, 0.0f);
		break;

	case TrapDir::Up:
		t1 = VGet(sx, sy + h, 0.0f);
		t2 = VGet(sx + w, sy + h, 0.0f);
		t3 = VGet(sx + w * 0.5f, sy, 0.0f);
		break;

	case TrapDir::Down:
		t1 = VGet(sx, sy, 0.0f);
		t2 = VGet(sx + w, sy, 0.0f);
		t3 = VGet(sx + w * 0.5f, sy + h, 0.0f);
		break;

	default:
		t1 = VGet(sx, sy, 0.0f);
		t2 = VGet(sx + w, sy, 0.0f);
		t3 = VGet(sx, sy + h, 0.0f);
		break;
	}
}

void FlyingSpikeTrap::UpdateShots(Player* player, Field* field)
{
	if (shots.empty()) return;

	// プレイヤー円（64前提）
	const float pw = 64.0f;
	const float ph = 64.0f;
	VECTOR pCenter = VGet(player->GetX() + pw * 0.5f, player->GetY() + ph * 0.5f, 0.0f);
	float pRadius = pw * 0.5f;

	for (auto& s : shots)
	{
		if (!s.alive) continue;

		// 移動
		s.x += s.vx;
		s.y += s.vy;

		// 当たり判定（円 vs 三角形）
		VECTOR t1, t2, t3;
		BuildTriangleVertices(s, t1, t2, t3);

		if (HitCheck_Circle_Triangle(pCenter, pRadius, t1, t2, t3))
		{
			PlaySoundMem(SpikeSE, DX_PLAYTYPE_BACK);
			ChangeVolumeSoundMem(70, SpikeSE);
			player->ForceDie();
			player->SetDead();
			s.alive = false;
			continue;
		}

		// 壁で消える（Fieldがあれば）
		if (field)
		{
			int tileX = int(s.x + spikeW * 0.5f) / 64;
			int tileY = int(s.y + spikeH * 0.5f) / 64;
			if (field->IsBlock(tileX, tileY))
			{
				s.alive = false;
				continue;
			}
		}

		// 画面外で消える（ワールドが広いなら調整）
		if (s.x < -spikeW * 4 || s.x > 5000 + spikeW * 4 ||
			s.y < -spikeH * 4 || s.y > 5000 + spikeH * 4)
		{
			s.alive = false;
			continue;
		}
	}

	shots.erase(
		std::remove_if(shots.begin(), shots.end(),
			[](const SpikeShot& s) { return !s.alive; }),
		shots.end()
	);
}

void FlyingSpikeTrap::Update()
{
	Player* player = FindGameObject<Player>();
	if (!player) return;

	Field* field = FindGameObject<Field>();

	// 弾は常に更新
	UpdateShots(player, field);

	// すでに発動済みなら、以後は発射しない
	if (triggered) return;

	bool inside = IsPlayerInside(player);

	// 入った瞬間だけ1回
	if (inside && !wasInside)
	{
		Fire(player);
		triggered = true;
	}

	wasInside = inside;
}

void FlyingSpikeTrap::Draw()
{
	// 弾描画
	for (const auto& s : shots)
	{
		if (!s.alive) continue;

		float cx = s.x + spikeW * 0.5f;
		float cy = s.y + spikeH * 0.5f;

		DrawRotaGraphF(cx, cy, 1.0, AngleFromDir(s.dir), sImage, TRUE);
	}

#ifdef _DEBUG
	// トリガー範囲（デバッグ表示）
	//DrawBox((int)x, (int)y, (int)(x + w), (int)(y + h), GetColor(255, 0, 255), FALSE);
#endif
}
