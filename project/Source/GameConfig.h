#pragma once

enum class Difficulty
{
	Easy,
	Hard
};

// 他シーンからの参照する共有値
inline Difficulty& SelectedDifficulty()
{
	static Difficulty d = Difficulty::Easy;
	return d;
}

inline int& MaxLives()
{
	static int lives = 5;
	return lives;
}