/* 
// Copyright 2007 Alexandros Panagopoulos
//
// This software is distributed under the terms of the GNU Lesser General Public Licence
//
// This file is part of Be3D library.
//
//    Be3D is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Be3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with Be3D.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _CRT_SECURE_NO_WARNINGS

#include "Console.h"

HANDLE Console::m_hConsole = INVALID_HANDLE_VALUE;

bool Console::create()
{
	free();

	if (!AllocConsole())
		return false;

	// Create the actual console
	m_hConsole = CreateFile(TEXT("CONOUT$"), GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	if (m_hConsole == INVALID_HANDLE_VALUE)
		return false;
	
	if (SetConsoleMode(m_hConsole, ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT ) == 0)
		return false;

	return true;
}

void Console::free()
{
	if (m_hConsole != INVALID_HANDLE_VALUE) {
		FreeConsole();
		m_hConsole = INVALID_HANDLE_VALUE;
	}
}

void Console::print(char* str, ...)
{
	if (m_hConsole != INVALID_HANDLE_VALUE)
	{
		char buf[1024];
		
		va_list va;
		va_start(va, str);
		vsprintf(buf, str, va);
		va_end(va);

#ifdef _UNICODE		
		DWORD strLen = (DWORD)strlen(buf);
		wchar_t bufw[1024];
		for (DWORD i=0; i<strLen; ++i)
			mbtowc(&bufw[i], &buf[i], 1);
		DWORD dwN;
		WriteConsole(m_hConsole, bufw, strLen, &dwN, 0);
#else
		DWORD strLen = (DWORD)strlen(buf);
		WriteConsole(m_hConsole, buf, (DWORD)strlen(buf), &dwN, 0);
#endif
	}
	else
	{
		va_list va;
		va_start(va, str);
		vprintf(str, va);
		va_end(va);
	}
}

void Console::print(const std::string &s)
{
	print("%s", s.c_str());
}

void Console::error(char* str, ...)
{
	if (m_hConsole != INVALID_HANDLE_VALUE)
	{
		char buf[1024];
		
		va_list va;
		va_start(va, str);
		vsprintf(buf, str, va);
		va_end(va);

#ifdef _UNICODE		
		DWORD strLen = (DWORD)strlen(buf);
		wchar_t bufw[1024];
		for (DWORD i=0; i<strLen; ++i)
			mbtowc(&bufw[i], &buf[i], 1);
		DWORD dwN;
		WriteConsole(m_hConsole, bufw, strLen, &dwN, 0);
#else
		DWORD strLen = (DWORD)strlen(buf);
		WriteConsole(m_hConsole, buf, (DWORD)strlen(buf), &dwN, 0);
#endif
	}
	else
	{
		va_list va;
		va_start(va, str);
		vprintf(str, va);
		va_end(va);
	}
}

void Console::error(const std::string &s)
{
	error("%s", s.c_str());
}