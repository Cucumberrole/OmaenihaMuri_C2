#pragma once
#include <DxLib.h>
#include <unordered_map>
#include <string>

class SoundCache
{
public:
	// 同じパスは1回だけLoadSoundMemして、同じハンドルを返す
	static int Get(const char* path)
	{
		auto& m = Map();
		auto it = m.find(path);
		if (it != m.end()) return it->second;

		const int h = LoadSoundMem(path);
		m.emplace(path, h);
		return h;
	}

	// 音量も設定したい時
	static int GetWithVolume(const char* path, int volume)
	{
		int h = Get(path);
		if (h >= 0) ChangeVolumeSoundMem(volume, h);
		return h;
	}

	// ゲーム終了時に一括解放
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
};
