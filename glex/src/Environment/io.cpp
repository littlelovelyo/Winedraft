#include "io.h"
#ifdef _DEBUG
#include <iostream>
#endif

using namespace glex;

std::string File::ReadAllText(char const* file)
{
	File f(file, File::ReadOnly);
	std::string text = f.ReadAllText();
	return text;
}

File::File(char const* file, OpenMode openMode)
{
	m_hFile = CreateFileA(file, static_cast<DWORD>(openMode),
		openMode == ReadOnly ? FILE_SHARE_READ : NULL, NULL,
		openMode == ReadOnly ? OPEN_EXISTING : OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
#ifdef _DEBUG
		std::cout << GLEX_LOG_ERROR "Error from I/O manager: failed to open " << file << ". Error code: " << GetLastError() << ".\n";
#endif
		throw Exception("IO error: failed to open file.");
	}
}

File::~File()
{
	CloseHandle(m_hFile);
}

bool File::Read(uint8_t* buffer, uint32_t size)
{
	DWORD bytesRead;
	if (!ReadFile(m_hFile, buffer, size, &bytesRead, NULL))
		throw Exception("Failed to read file");
	return bytesRead == size;
}

std::string File::ReadAllText()
{
	SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
	DWORD size = GetFileSize(m_hFile, NULL);
	std::string ret(size, 0);
	Read((uint8_t*)ret.data(), size);
	return ret;
}

void File::DiscardUntil(char chr)
{
	char red;
	while (TryRead(red) && red != chr)
		continue;
}

void File::ReadUntil(uint8_t* buffer, uint32_t size, char chr)
{
	for (uint32_t i = 0; i < size; i++)
	{
		if (!TryRead(buffer[i]) || buffer[i] == chr)
			break;
	}
}