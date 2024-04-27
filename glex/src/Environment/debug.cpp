#ifdef _DEBUG
#include "memory.h"
#include "debug.h"
#include <Windows.h>
#include <DbgHelp.h>
#include <iostream>
#include <intrin.h>

#pragma comment(lib, "Dbghelp.lib")

using namespace glex;

void Debug::Init()
{
	if (!SymInitialize(reinterpret_cast<HANDLE>(1), nullptr, FALSE))
		throw Exception("Failed to initialize DbgHelp.");
	SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(StackAllocator::Allocate(512));
	memset(symbol, 0, 512);
	symbol->SizeOfStruct = 512;
	symbol->MaxNameLen = 128;
	SymFromAddr(reinterpret_cast<HANDLE>(1), reinterpret_cast<uint64_t>(_ReturnAddress()), 0, symbol);
	std::cout << symbol->Name << std::endl;
}
#endif