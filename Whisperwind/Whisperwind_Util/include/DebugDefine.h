/*-------------------------------------------------------------------------
This source file is a part of Whisperwind.(GameEngine + GamePlay + GameTools)
For the latest info, see http://lisuyong.com

Copyright (c) 2012 Suyong Li (79481268@qq.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE
-------------------------------------------------------------------------*/
#ifndef _DEBUG_DEFINE_H_
#define _DEBUG_DEFINE_H_

#include "Util.h"

#ifdef WHISPERWIND_DEBUG

//---------------------------------------------------------------------
	#include <iostream>
	#include <boost/io/ios_state.hpp>

	#define DEBUG_PRINT(x) \
		{ \
		    boost::io::wios_all_saver ios(std::wcout); \
			std::wcout.imbue(std::locale(std::locale(), "", LC_CTYPE)); \
			std::wcout << (x) << std::endl; \
		}

//---------------------------------------------------------------------
	/** for windows.h's warning level */
	#pragma warning(push, 3)
	#include <windows.h>
	#pragma warning(pop)

	#define  DEBUG_PRINT_RED(x) \
	{ \
		HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE); \
		CONSOLE_SCREEN_BUFFER_INFO oldInfo; \
		::GetConsoleScreenBufferInfo(handle, &oldInfo ); \
		::SetConsoleTextAttribute(handle,FOREGROUND_INTENSITY | FOREGROUND_RED); \
		DEBUG_PRINT(x) \
		::SetConsoleTextAttribute(handle, oldInfo.wAttributes); \
	}

//---------------------------------------------------------------------
	#define BOOST_ENABLE_ASSERT_HANDLER
	#include <boost/assert.hpp>

	#include <boost/format.hpp>
	#include <iostream>
	namespace boost
	{
		inline void assertion_failed(char const * expr, char const * function, char const * file, long line)
		{
			boost::wformat wfmt(TO_UNICODE("Assertion Failed!\nExpression: %s\nFunction: %s\nFile: %s\nLine: %ld\n\n"));
			wfmt % expr% function% file% line;
			DEBUG_PRINT(wfmt);
			::DebugBreak();
		}
	}

	#define WHISPERWIND_ASSERT(x) BOOST_ASSERT(x);

#else

	#define DEBUG_PRINT(x) (x)
	#define DEBUG_PRINT_RED(x) (x)
	#define  WHISPERWIND_ASSERT(x) (0)

#endif // WHISPERWIND_DEBUG

#endif // header