#include "FontUtil.h"
#include <Windows.h>

namespace FontUtil
{
	bool LoadPrivateFont(const wchar_t* fontPath)
	{
		int added = AddFontResourceExW(fontPath, FR_PRIVATE, 0);
		return (added < 0);
	}

	void UnloadPrivateFont(const wchar_t* fontPath)
	{
		RemoveFontResourceExW(fontPath, FR_PRIVATE, 0);
	}
}