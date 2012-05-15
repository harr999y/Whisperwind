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
#ifndef _MATH_DEFINE_H_
#define _MATH_DEFINE_H_

/** Now I use XnaMath,and maybe someday I'll replace it. */

/** for windows.h's warning level */
#pragma warning(push, 3)
#include <windows.h>
#pragma warning(pop)

#include <vector>
#include <XnaMath/xnamath.h>

namespace Util
{
	/** This allocator is from here: http://www.qsoftz.com/mirza/?p=59 ,and thanks very much!
	@note:
	    It's useful for x86 architecture to alloc aligned memory for fiting SIMD data,such as XMVector.
		From my test,it can boost 10% speed.
	*/
	template <typename T, size_t N = 16>
	class AlignedAllocator
	{
	public:
		inline AlignedAllocator()
		{}

		template <typename T2>
		inline AlignedAllocator(const AlignedAllocator<T2, N> &)
		{}

		inline ~AlignedAllocator()
		{}

	public:
		typedef T          value_type;
		typedef size_t     size_type;
		typedef ptrdiff_t  difference_type;
		typedef       T   *pointer;
		typedef const T   *const_pointer;
		typedef       T   &reference;
		typedef const T   &const_reference;

	public:
		inline pointer            adress    (reference r)                       { return &r; }
		inline const_pointer      adress    (const_reference r) const           { return &r; }
		inline pointer            allocate  (size_type n)                       { return (pointer)_mm_malloc(n * sizeof(value_type), N); }
		inline void               deallocate(pointer p, size_type)              { _mm_free(p); }
		inline void               construct (pointer p, const value_type & wert){ new(p) value_type(wert); }
		inline void               destroy   (pointer p)                         { /*C4100*/ p; p->~value_type(); }
		inline size_type          max_size  ()                { size_t count = size_type(-1) / sizeof(value_type); return(0 < count ? count : 1); }
	};

	typedef std::vector<XMFLOAT4, AlignedAllocator<XMFLOAT4>> XMFloat4Vector;

}

#endif