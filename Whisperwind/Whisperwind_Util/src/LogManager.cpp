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

#define BOOST_ALL_NO_LIB /// Don't use boost lib.
#include <boost/date_time/posix_time/posix_time.hpp>

#include "DebugDefine.h"
#include "StringConverter.h"
#include "LogManager.h"

namespace Util
{
	//---------------------------------------------------------------------
	LogManager LogManager::mSingleton;
	LogManager & LogManager::getSingleton()
	{
		return mSingleton;
	}
	//---------------------------------------------------------------------
	LogManager::LogManager()
	{
		/// As global initialise is random,I put this string here.
		static const String LOG_PATH("../Whisperwind.log");

		mStream.open(LOG_PATH, std::ios::out | std::ios::trunc);
		WHISPERWIND_ASSERT(mStream.is_open());
	}
	//---------------------------------------------------------------------
	LogManager::~LogManager()
	{
		mStream.close();
	}
	//---------------------------------------------------------------------
	void LogManager::log(const Wstring & event)
	{
		/// I don't know why this has nothing effect to me...
		//std::locale::global(std::locale(""));

		/// So finally I just choose the common way to handle this with ANSI format.
		/// NOTE:I'll fix it.
		/// TODO:Fix it.

		String strTime = boost::posix_time::to_iso_extended_string(
			boost::posix_time::second_clock::local_time());		
		strTime[10] = ' ';

		mStream  << (strTime + " : " + WstringToString(event)).c_str() << std::endl;
		mStream.flush();

		DEBUG_PRINT(event);
	}
}