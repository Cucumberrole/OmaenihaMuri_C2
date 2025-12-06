#include <DxLib.h>
#include "Trap.h"
#include "Player.h"

//--------------------------------------
// コンストラクタ
//--------------------------------------
Trap::Trap(int sx, int sy)
{
	hImage = LoadGraph("data/image/hari.png"); // 針画像
	x = sx;
	y = sy;
	width = 64;
	height = 64;

	offsetY = 0.0f;
	isActive = false;
	isExtended = false;
	moveSpeed = 10.0f; // 上にせり出すスピード

	SetDrawOrder(100); // 描画順序
}

//--------------------------------------
// デストラクタ
//--------------------------------------
Trap::~Trap()
{
	DeleteGraph(hImage);
}

//--------------------------------------
// 円と線分の当たり判定
//--------------------------------------
bool HitCheck_Circle_Line(VECTOR center, float radius, VECTOR a, VECTOR b)
{
	VECTOR ab = VSub(b, a);
	VECTOR ac = VSub(center, a);

	float abLen2 = ab.x * ab.x + ab.y * ab.y;
	if (abLen2 <= 0.0f) return false; // 同一点保険

	float t = (ab.x * ac.x + ab.y * ac.y) / abLen2;
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;

	VECTOR closest = VAdd(a, VScale(ab, t));

	float dx = center.x - closest.x;
	float dy = center.y - closest.y;

	return (dx * dx + dy * dy <= radius * radius);
}

//--------------------------------------
// 点が三角形の中かどうか
//--------------------------------------
bool PointInTriangle(VECTOR p, VECTOR a, VECTOR b, VECTOR c)
{
	VECTOR v0 = VSub(c, a);
	VECTOR v1 = VSub(b, a);
	VECTOR v2 = VSub(p, a);

	float dot00 = VDot(v0, v0);
	float dot01 = VDot(v0, v1);
	float dot02 = VDot(v0, v2);
	float dot11 = VDot(v1, v1);
	float dot12 = VDot(v1, v2);

	float denom = (dot00 * dot11 - dot01 * dot01);
	if (denom == 0.0f) return false; // 退化三角形保険

	float invDenom = 1.0f / denom;
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	return (u >= 0) && (v >= 0) && (u + v < 1);
}

//--------------------------------------
// 円と三角形の当たり判定
//--------------------------------------
bool HitCheck_Circle_Triangle(
	VECTOR center, float radius,
	VECTOR t1, VECTOR t2, VECTOR t3)
{
	// 円と三辺
	if (HitCheck_Circle_Line(center, radius, t1, t2)) return true;
	if (HitCheck_Circle_Line(center, radius, t2, t3)) return true;
	if (HitCheck_Circle_Line(center, radius, t3, t1)) return true;

	// 円の中心が三角形内部にある場合
	if (PointInTriangle(center, t1, t2, t3)) return true;

	return false;
}

//--------------------------------------
// Update
//--------------------------------------
void Trap::Update()
{
	Player* player = FindGameObject<Player>();
	if (player == nullptr) {
		return;
	}

	float px = player->GetX();
	float py = player->GetY();
	float pw = 64.0f;
	float ph = 64.0f;

	//----------------------------------
	// プレイヤーが「上のブロック」を踏んだら発動
	// （針の真上 1マス分を判定エリアとする）
	//----------------------------------
	if (!isActive && !isExtended)
	{
		float triggerTop = y - height; // 針の真上の1マス上
		float triggerBottom = y;          // 針の床高さ
		float playerFootY = py + ph;    // プレイヤーの足元

		bool inX =
			(px + pw > x) &&
			(px < x + width);

		bool inY =
			(playerFootY > triggerTop) &&
			(playerFootY <= triggerBottom);

		if (inX && inY)
		{
			Activate();
		}
	}

	//----------------------------------
	// トラップ上昇アニメーション
	//----------------------------------
	if (isActive && !isExtended)
	{
		offsetY += moveSpeed;
		if (offsetY >= height)
		{
			offsetY = (float)height;
			isActive = false;
			isExtended = true;   // 出きった
		}
	}

	// ----------------------------------
	// 当たり判定（出ている間だけ）
	// ----------------------------------
	if (isExtended)
	{
		float baseY = y - offsetY;

		// 三角形の頂点
		VECTOR tri1 = VGet(x, baseY + height, 0); // 左下
		VECTOR tri2 = VGet(x + width, baseY + height, 0); // 右下
		VECTOR tri3 = VGet(x + width / 2, baseY, 0); // 上の先端

		// プレイヤーの円当たり判定を取得
		float cx, cy, cr;
		player->GetHitCircle(cx, cy, cr);

		VECTOR center = VGet(cx, cy, 0);
		float  radius = cr;

		if (HitCheck_Circle_Triangle(center, radius, tri1, tri2, tri3))
		{
			player->ForceDie();
			player->SetDead();
		}
	}
}

//--------------------------------------
// Activate
//--------------------------------------
void Trap::Activate()
{
	isActive = true;
}

//--------------------------------------
// Draw
//--------------------------------------
void Trap::Draw()
{
	// offsetY 分だけ上に描画
	DrawRectGraph(
		static_cast<int>(x),
		static_cast<int>(y - offsetY),
		0, 0, width, height,
		hImage,
		TRUE
	);
}

//--------------------------------------
// 旧AABB判定（今は使っていなければ不要）
//--------------------------------------
bool Trap::CheckHit(int px, int py, int pw, int ph)
{
	return (px + pw > x && px < x + width &&
		py + ph > y - offsetY && py < y + height);
}