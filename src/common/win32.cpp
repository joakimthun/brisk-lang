#include "win32.h"

#include <Windows.h>
#include <tchar.h>

#include "exceptions.h"
#include "util.h"

namespace brisk {
	namespace win32 {

		std::unique_ptr<u8> utf8_encode(const WCHAR *wstr, std::size_t length)
		{
			auto size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, static_cast<int>(length), nullptr, 0, nullptr, nullptr);

			auto result = std::unique_ptr<u8>(static_cast<u8*>(calloc(size_needed + 1, 1)));
			result.get()[size_needed] = '\0';

			WideCharToMultiByte(CP_UTF8, 0, wstr, static_cast<int>(length), (char*)result.get(), size_needed, nullptr, nullptr);

			return result;
		}

		std::unique_ptr<WCHAR> utf8_decode(const char *str, std::size_t length)
		{
			auto size_needed = MultiByteToWideChar(CP_UTF8, 0, str, (int)length, nullptr, 0);

			auto result = std::unique_ptr<WCHAR>(static_cast<WCHAR*>(calloc((size_needed + 1) * sizeof(WCHAR), 1)));
			result.get()[size_needed] = L'\0';

			MultiByteToWideChar(CP_UTF8, 0, str, static_cast<int>(length), result.get(), size_needed);

			return result;
		}

		std::unique_ptr<u8> get_full_path_name(const std::string &path)
		{
			TCHAR  buffer[MAX_PATH] = { 0 };
			TCHAR** lppPart = { 0 };

			auto wstr = utf8_decode(path.c_str(), path.size());

			// Retrieve the full path name for a file, the file does not need to exist
			auto result = GetFullPathName(wstr.get(), MAX_PATH, buffer, lppPart);

			if (result)
				return utf8_encode(buffer, result);

			throw BriskException("win32::get_full_path_name: Could not get path name");
		}

	}
}