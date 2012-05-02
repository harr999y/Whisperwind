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
#ifndef _UTIL_TYPEDEFS_H_
#define _UTIL_TYPEDEFS_H_

#include <vector>
#include <string>

#define TO_UNICODE(x) L##x

namespace Util
{
	/** The data defines. */
	typedef float real;
	typedef double double_real;
	typedef unsigned __int32 u_int;
	typedef signed __int32 s_int;
	typedef u_int size_t;
	typedef unsigned __int64 u_int64;
	typedef signed __int64 s_int64;
	typedef unsigned __int16 u_int16;
	typedef signed __int16 s_int16;
	typedef unsigned __int8 u_int8;
	typedef signed __int8 s_int8;

	typedef std::pair<Util::u_int, Util::u_int> UintPair;

	/** The common classes defines. */
	typedef std::string String;
	typedef std::wstring Wstring;
	typedef std::vector<String> StringVector;
	typedef std::vector<Wstring> WstringVector;

	/** Time. */
	typedef real time;
}

#endif