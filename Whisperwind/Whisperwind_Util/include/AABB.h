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
#ifndef _AABB_H_
#define _AABB_H_

#include "Util.h"
#include "MathDefine.h"

namespace Util
{
	class WHISPERWIND_API AABB
	{
	public:
		AABB() :
		    mMinPoint(0.0f, 0.0f, 0.0f),
			mMaxPoint(0.0f, 0.0f, 0.0f),
			mCenterPoint(0.0f, 0.0f, 0.0f),
			mIsInvalid(true)
		{}

		AABB(FXMVECTOR minPoint, FXMVECTOR maxPoint);

		~AABB()
		{}

	public:
		void merge(const AABBPtr & aabb);
		void merge(FXMVECTOR minPoint, FXMVECTOR maxPoint);
		void addPoint(FXMVECTOR point);
		void move(FXMVECTOR trans);
		void rotate(FXMVECTOR quat);
		void reset();

	public:
		XMVECTOR getMinPoint() { return XMLoadFloat3(&mMinPoint); }
		XMVECTOR getMaxPoint() { return XMLoadFloat3(&mMaxPoint); }
		XMVECTOR getCenterPoint() { return XMLoadFloat3(&mCenterPoint); }
		XMVECTOR getHalfSize() { return XMLoadFloat3(&mHalfSize); }
		XMVECTOR getSize() { return getMaxPoint() - getMinPoint(); }

		GET_CONST_VALUE(bool, IsInvalid);

	private:
		void calcCenterAndHalfSize();

	private:
		XMFLOAT3 mMinPoint;
		XMFLOAT3 mMaxPoint;
		XMFLOAT3 mCenterPoint;
		XMFLOAT3 mHalfSize;

		bool mIsInvalid;

	private:
		DISALLOW_COPY_AND_ASSIGN(AABB);
		BOOST_SERIALIZATION_NONINSTRUSIVE_FRIEND_FUNC(AABB);
	};

}

#endif