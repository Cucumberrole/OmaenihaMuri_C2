#include "Field.h"
#include<vector>
#include "Player.h"
using namespace std;

vector<vector<int>> maps = {
	//int maps[10][15] = {
					{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
					{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
					{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
					{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					{1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};
Field::Field()
{
	kabehImage = LoadGraph("data/image/kabe.png");
	hImage = LoadGraph("data/image/New Blo.png");
	x = 0;
	y = 0;
	scrollX = 0;
	for (int y = 0; y < maps.size(); y++)
	{
		for (int x = 0; x < maps[y].size(); x++)
		{
			if (maps[y][x] == 2)
			{
					new Player(x * 64, y * 64 + 400);
			}
		}
	}
	
	
}
Field::~Field()
{

}

void Field::Update()
{
	//scrollX += 1;//‹­§ƒXƒNƒ[ƒ‹‚Ìê‡

}

void Update()
{

}



void Field::Draw()
{
	DrawGraph(0, 0, kabehImage, TRUE);
	for (int y = 0; y < maps.size(); y++)
	{
		for (int x = 0; x < maps[y].size(); x++)
		{
			if (maps[y][x] == 1)
			{
				DrawRectGraph(x * 64 - scrollX, y * 64 + 2, 0, 0, 64, 64, hImage, 1);
			}
		}
	}
}


int Field::HitCheckRight(int px, int py)
{
	if (py < 400) {
		return 0;
	}
	int x = px / 64;
	int y = (py - 400) / 64;
	if (maps[y][x] == 1)
	{
		return px % 64 + 1;
	}
	return 0;
}

int Field::HitCheckLeft(int px, int py)
{
	if (py < 400) {
		return 0;
	}
	int x = (px / 64);
	int y = (py - 400) / 64;
	if (maps[y][x] == 1)
	{
		return 64 - px %64;
	}
	return 0;
}

int Field::HitCheckDown(int px, int py)
{
	if (py < 400) {
		return 0;
	}
	int x = px / 64;
	int y = (py - 400) / 64;
	if (maps[y][x] == 1)
	{
		return (py-400)%64+1;
	}
	return 0;
}

int Field::HitCheckUp(int px, int py)
{
	if (py < 400) {
		return 0;
	}
	int x = px / 64;
	int y = (py - 400) / 64;
	if (maps[y][x] == 1)
		return 64 -(py - 400) % 64;
	return 0;
}