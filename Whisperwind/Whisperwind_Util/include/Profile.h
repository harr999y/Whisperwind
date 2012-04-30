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
#ifndef _PROFILE_H_
#define _PROFILE_H_

#include "Util.h"

namespace Util
{
	/** Profile classes.
	@note
	    This file is transplanted from my other project,so the code style maybe 
		has a little differences.
	*/

	/** Base class for profile. */
	class WHISPERWIND_API Profile
	{
	public:
		/// Constructor.
		Profile();

	public:
		/// return profile result.
		s_int64 getResult() const;
		/// begin test.
		void beginTest();
		/// end test.
		void endTest();

	protected:
		/** Destructor.
		@note 
		    The rule set it to be virtual.
		*/
		virtual ~Profile()
		{}

	private:
		/** virtual function for profile algorithm implementation.
		@note
			Every derived class should implement this function as thier own way.
		*/
		virtual void beginTest_impl() = 0;

		/** virtual function for profile algorithm implementation.
		@note
			Every derived class should implement this function as thier own way.
		*/
		virtual void endTest_impl() = 0;

	protected:
		/// Last stamp.
		s_int64 mLastStamp;
		/// Current stamp.
		s_int64 mCurrentStamp;
	};
}

/** for windows.h's warning level */
#pragma warning(push, 3)
#include <windows.h>
#pragma warning(pop)

namespace Util
{
	/** Use Windows API to get cpu clock. */
	class WHISPERWIND_API ProfileWithWin32API : 
		public Profile
	{
	private:
		/// implement this function to begin test.
		virtual void beginTest_impl();
		/// implement this function to end test.
		virtual void endTest_impl();

		/// private method to call QueryPerformanceFrequency
		u_int64 queryPerfCount(LARGE_INTEGER time) const;
	};
}

#include "boost/lexical_cast.hpp"

#ifdef WHISPERWIND_DEBUG
#define PROFILE(x, y) \
	{ \
		Util::ProfileWithWin32API profile; \
		profile.beginTest(); \
		x; \
		profile.endTest(); \
		Util::s_int64 result = profile.getResult(); \
		WHISPERWIND_LOG(Util::Wstring(y) + TO_UNICODE(" profile's result is : ") + \
			boost::lexical_cast<Util::Wstring>(result) + TO_UNICODE(" MS.")); \
		BOOST_ASSERT(result >= 0 && "Profile getProfile Failed!"); \
    }
#else
#define PROFILE(x, y) (0);
#endif

#endif