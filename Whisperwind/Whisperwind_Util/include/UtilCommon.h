/*-------------------------------------------------------------------------
This source file is a part of Whisperwind(GameEngine + GamePlay + GameTools)
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
#ifndef _UTIL_COMMON_H_
#define _UTIL_COMMON_H_

#pragma warning(push, 4)

#include "UtilWarningDisable.h"
#include "UtilTypedefs.h"

/** for unicode */
#ifndef UNICODE
    #ifndef _UNICODE
        #define _UNICODE
    # endif
    #define UNICODE
#endif

#if defined(DEBUG) || defined(_DEBUG)
    #define WHISPERWIND_DEBUG
#endif

#define BOOST_SYSTEM_NO_LIB /// Don't auto link boost lib.

#include <memory> // for shared_ptr
#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"

#include "MemoryDefines.h"

#define SAFE_DELETE(x) \
	if (x) { \
	WHISPERWIND_DELETE(x); \
	(x) = NULL; }

#define SAFE_DELETE_ARRAY(x) \
	if (x) { \
	WHISPERWIND_DELETE_ARRAY(x); \
	(x) = NULL; }

#define SAFE_RELEASE(x) \
	if (x) { \
	(x)->Release(); \
	(x) = NULL; }

#include "ExceptionDefines.h"

#define IF_NULL_RETURN(x) \
	if (!(x)) return;
#define IF_NULL_RETURN_VALUE(x, y) \
	if (!(x)) return (y);
#define IF_NULL_CONTINUE(x) \
	if (!(x)) continue;
#define IF_NULL_BREAK(x) \
	if (!(x)) break;
#define IF_NULL_EXCEPTION(x, y) \
	if (!(x)) EXCEPTION((y));
#define IF_FALSE_EXCEPTION(x, y) \
	if (!(x)) EXCEPTION((y));

#ifdef WHISPERWIND_DEBUG
    #define DEBUG_PRINT(x) \
	::OutputDebugString(TO_UNICODE(x))
#else
    #define DEBUGPRINT(x) (0)
#endif

	/** Define DISALLOW_COPY_AND_ASSIGN macro for copy-constructor
	and operator =. */
#define DISALLOW_COPY_AND_ASSIGN(Type)\
	Type(Type const &);\
	void operator=(Type const &);

#ifdef DLL_AS_EXPORT
    #define WHISPERWIND_API _declspec(dllexport)
#else
    #define WHISPERWIND_API _declspec(dllimport)
#endif

#define SET_GET_VALUE(type, name) \
	inline void set##name(const type & val) { m##name = val; } \
	inline type & get##name() { return m##name; }

#define SET_GET_CONST_VALUE(type, name) \
	inline void set##name(const type & val) { m##name = val; } \
	inline const type & get##name() const { return m##name; }

#endif
