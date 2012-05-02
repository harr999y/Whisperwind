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

#include "Profile.h"
#include "ExceptionDefines.h"

namespace Util
{
	//---------------------------------------------------------------------
	// Profile
	//---------------------------------------------------------------------
	Profile::Profile() :
	    mLastStamp(0),
	    mCurrentStamp(0)
	{}
	//---------------------------------------------------------------------
	void Profile::beginTest()
	{
		beginTest_impl();
	}
	//---------------------------------------------------------------------
	void Profile::endTest()
	{	
		endTest_impl();
	}
	//---------------------------------------------------------------------
	s_int64 Profile::getResult() const
	{
		return mCurrentStamp - mLastStamp;
	}

	//---------------------------------------------------------------------
	// ProfileWithWin32API
	//---------------------------------------------------------------------
	void ProfileWithWin32API::beginTest_impl()
	{
		// set highest thread ptiority to get more precise time.
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

		LARGE_INTEGER beginTime;
		IF_NULL_EXCEPTION(QueryPerformanceCounter(&beginTime), "QueryPerformanceCounter failed!");

		mLastStamp = queryPerfCount(beginTime);
	}
	//---------------------------------------------------------------------
	void ProfileWithWin32API::endTest_impl()
	{
		LARGE_INTEGER endTime;
		IF_NULL_EXCEPTION(QueryPerformanceCounter(&endTime), "QueryPerformanceCounter failed!");

		mCurrentStamp = queryPerfCount(endTime);

		// set thread ptiority back to normal.
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	}
	//---------------------------------------------------------------------
	/** The time factor. */
#define TF_MICRO 1000000
#define TF_MILLI 1000

	u_int64 ProfileWithWin32API::queryPerfCount(LARGE_INTEGER time) const
	{
		LARGE_INTEGER perfFreq;
		IF_NULL_EXCEPTION(QueryPerformanceFrequency(&perfFreq), "QueryPerformanceFrequency failed!");

		return ((time.QuadPart * TF_MILLI) / perfFreq.QuadPart);
	}
}