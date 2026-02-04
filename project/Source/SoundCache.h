#pragma once
#include <DxLib.h>
#include <unordered_map>
#include <string>

class SoundCache
{
public:
	static void SetDefaultVolume(int v)
	{
		DefaultVolume() = v;
		// すでにロード済みの分も反映
		for (auto& kv : Map())
		{
			if (kv.second >= 0) ChangeVolumeSoundMem(v, kv.second);
		}
	}

	static int Get(const char* path)
	{
		auto& m = Map();
		auto it = m.find(path);
		if (it != m.end()) return it->second;

		const int h = LoadSoundMem(path);
		m.emplace(path, h);

		if (h >= 0) ChangeVolumeSoundMem(DefaultVolume(), h); // ここで統一
		return h;
	}

	// 個別で音量操作が欲しいときだけ
	static int GetWithVolume(const char* path, int volume)
	{
		int h = Get(path);
		if (h >= 0) ChangeVolumeSoundMem(volume, h);
		return h;
	}

	static void ReleaseAll()
	{
		auto& m = Map();
		for (auto& kv : m)
		{
			if (kv.second >= 0) DeleteSoundMem(kv.second);
		}
		m.clear();
	}

private:
	static std::unordered_map<std::string, int>& Map()
	{
		static std::unordered_map<std::string, int> s_map;
		return s_map;
	}
	static int& DefaultVolume()
	{
		static int v = 128; // デフォルト
		return v;
	}
};
