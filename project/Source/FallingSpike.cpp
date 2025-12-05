#include "FallingSpike.h"
#include "Field.h"
#include "Player.h"
#include <DxLib.h>

//--------------------------------------
// 円と線分の当たり判定
//--------------------------------------
static bool HitCheck_Circle_Line(VECTOR center, float radius, VECTOR a, VECTOR b)
{
	VECTOR ab = VSub(b, a);
	VECTOR ac = VSub(center, a);

	float abLen2 = ab.x * ab.x + ab.y * ab.y;
	if (abLen2 <= 0.0001f) return false;

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
static bool PointInTriangle(VECTOR p, VECTOR a, VECTOR b, VECTOR c)
{
	VECTOR v0 = VSub(c, a);
	VECTOR v1 = VSub(b, a);
	VECTOR v2 = VSub(p, a);

	float dot00 = VDot(v0, v0);
	float dot01 = VDot(v0, v1);
	float dot02 = VDot(v0, v2);
	float dot11 = VDot(v1, v1);
	float dot12 = VDot(v1, v2);

	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	return (u >= 0.0f) && (v >= 0.0f) && (u + v < 1.0f);
}

//--------------------------------------
// 円と三角形の当たり判定
//--------------------------------------
static bool HitCheck_Circle_Triangle(VECTOR center, float radius,
	VECTOR t1, VECTOR t2, VECTOR t3)
{
	if (HitCheck_Circle_Line(center, radius, t1, t2)) return true;
	if (HitCheck_Circle_Line(center, radius, t2, t3)) return true;
	if (HitCheck_Circle_Line(center, radius, t3, t1)) return true;

	if (PointInTriangle(center, t1, t2, t3)) return true;

	return false;
}

//--------------------------------------
// コンストラクタ
//--------------------------------------
FallingSpike::FallingSpike(int sx, int sy)
{
	hImage = LoadGraph("data/image/hariBottom.png");

	x = sx;
	y = sy;

	vy = 0.0f;
	gravity = 0.8f;
	landed = false;

	// 画像サイズを取得して width / height に保存
	int w, h;
	GetGraphSize(hImage, &w, &h);
	width = w;
	height = h;
}

//--------------------------------------
// デストラクタ
//--------------------------------------
FallingSpike::~FallingSpike()
{
	DeleteGraph(hImage);
}

//--------------------------------------
// Update
//--------------------------------------
void FallingSpike::Update()
{
	// まだ落下中なら重力処理
	if (!landed)
	{
		vy += gravity;
		y += vy;

		// 地面に衝突判定
		Field* field = FindGameObject<Field>();
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

	// プレイヤー衝突判定（落下中・着地後どちらでも有効）
	Player* player = FindGameObject<Player>();
	if (player)
	{
		float px = player->GetX();
		float py = player->GetY();

		// プレイヤーの当たり判定「円」（中心 + 半径）
		VECTOR center = VGet(px + 32.0f, py + 32.0f, 0.0f);
		float  radius = player->GetRadius();   // 64 * 0.35f 相当

		//  左上, 右上, 下の先端
		VECTOR t1 = VGet(x, y, 0.0f); // 左上
		VECTOR t2 = VGet(x + width, y, 0.0f); // 右上
		VECTOR t3 = VGet(x + width / 2, y + height, 0.0f); // 下の先端

		if (HitCheck_Circle_Triangle(center, radius, t1, t2, t3))
		{
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
	// 普通の描画
	DrawGraph(static_cast<int>(x), static_cast<int>(y), hImage, TRUE);

#ifdef _DEBUG
	int tx1 = static_cast<int>(x);
	int ty1 = static_cast<int>(y); // 左上

	int tx2 = static_cast<int>(x + width);
	int ty2 = static_cast<int>(y); // 右上

	int tx3 = static_cast<int>(x + width / 2);
	int ty3 = static_cast<int>(y + height); // 下の先端

	int col = GetColor(0, 255, 0);

	DrawTriangle(tx1, ty1, tx2, ty2, tx3, ty3, col, FALSE);
#endif
}