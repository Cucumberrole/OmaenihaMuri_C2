#include "Collision.h"

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

bool HitCheck_Circle_Line(VECTOR center, float radius, VECTOR a, VECTOR b)
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

bool HitCheck_Circle_Triangle(VECTOR center, float radius, VECTOR t1, VECTOR t2, VECTOR t3)
{
	if (HitCheck_Circle_Line(center, radius, t1, t2)) return true;
	if (HitCheck_Circle_Line(center, radius, t2, t3)) return true;
	if (HitCheck_Circle_Line(center, radius, t3, t1)) return true;

	if (PointInTriangle(center, t1, t2, t3)) return true;

	return false;
}