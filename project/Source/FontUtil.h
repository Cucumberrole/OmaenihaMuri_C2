#pragma once

namespace FontUtil
{
	bool LoadPrivateFont(const wchar_t* fontPath);

	void UnloadPrivateFont(const wchar_t* fontPath);
}