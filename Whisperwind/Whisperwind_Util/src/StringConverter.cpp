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

#include "UtilWarningDisable.h"

/** for windows.h's warning level */
#pragma warning(push, 3)
#include <windows.h>
#pragma warning(pop)

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "DebugDefine.h"
#include "ExceptionDefine.h"
#include "StringConverter.h"

namespace Util
{
	Wstring StringToWstring(const String & src)
	{
		u_int const wcsLen = MultiByteToWideChar(CP_ACP, 0, src.c_str(), static_cast<u_int>(src.size()), NULL, 0);
		std::vector<wchar_t> tmpVec(wcsLen + 1);
		MultiByteToWideChar(CP_ACP, 0, src.c_str(), static_cast<u_int>(src.size()), &tmpVec[0], wcsLen);

		return Wstring(tmpVec.begin(), tmpVec.end() - 1);
	}

	String WstringToString(const Wstring & src)
	{
		u_int const mbsLen = WideCharToMultiByte(CP_ACP, 0, src.c_str(), static_cast<u_int>(src.size()), NULL, 0, NULL, NULL);
		std::vector<char> tmpVec(mbsLen + 1);
		WideCharToMultiByte(CP_ACP, 0, src.c_str(), static_cast<u_int>(src.size()), &tmpVec[0], mbsLen, NULL, NULL);

		return String(tmpVec.begin(), tmpVec.end() - 1);
	}

	XMVECTOR StringToVector(const String & str, size_t column)
	{
		XMVECTOR vec = XMVectorZero();
		try
		{
			Util::StringVector strVec;
			boost::algorithm::split(strVec, str, boost::is_any_of(","));

			WHISPERWIND_ASSERT((strVec.size()) == column);

			for (size_t it = 0; it < column; ++it)
			{
				vec = XMVectorSetByIndex(vec, boost::lexical_cast<real>(strVec[it]), it);
			}
		}
		catch (boost::exception & e)
		{
			DEBUG_PRINT_RED(boost::diagnostic_information_what(e));
		}

		return vec;
	}

	String VectorToString(FXMVECTOR vector, size_t column)
	{
		String str("");
		try
		{
			for (size_t it = 0; it < column; ++it)
			{
				str += boost::lexical_cast<String>(XMVectorGetByIndex(vector, it)) + ",";
			}

			boost::algorithm::erase_last(str, ",");
		}
		catch (boost::exception & e)
		{
			DEBUG_PRINT_RED(boost::diagnostic_information_what(e));
		}

		return str;
	}

}