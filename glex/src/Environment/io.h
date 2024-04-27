#pragma once
#include "comhdr.h"
#include <Windows.h>
#undef CreateFont
#undef DrawText
#include <string>

namespace glex
{
	class File
	{
	public:
		static std::string ReadAllText(char const* file);
	private:
		HANDLE m_hFile = INVALID_HANDLE_VALUE;
	public:
		enum OpenMode : DWORD {
			ReadOnly = FILE_GENERIC_READ,
			WriteOnly = FILE_GENERIC_WRITE,
			ReadWrite = ReadOnly | WriteOnly
		};
		File(char const* file, OpenMode openMode);
		~File();
		bool Read(uint8_t* buffer, uint32_t size);
		std::string ReadAllText();
		void DiscardUntil(char chr);
		void ReadUntil(uint8_t* buffer, uint32_t size, char chr);

		template <typename T>
		T Read()
		{
			T ret;
			Read(reinterpret_cast<uint8_t*>(&ret), sizeof(T));
			return ret;
		}

		template <typename T>
		bool TryRead(T& result)
		{
			return Read(reinterpret_cast<uint8_t*>(&result), sizeof(T));
		}
	};
}