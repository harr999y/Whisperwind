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

#include "AABB.h"

namespace Util
{
	//---------------------------------------------------------------------
	AABB::AABB(FXMVECTOR minPoint, FXMVECTOR maxPoint) :
        mIsInvalid(false)
	{
		XMStoreFloat3(&mMinPoint, minPoint);
		XMStoreFloat3(&mMaxPoint, maxPoint);

		calcCenterPoint();
	}
	//---------------------------------------------------------------------
	void AABB::merge(const AABBPtr & aabb)
	{
		if (aabb->getIsInvalid())
			return;

		merge(XMLoadFloat3(&aabb->getMinPoint()), XMLoadFloat3(&aabb->getMaxPoint()));
	}
	//---------------------------------------------------------------------
	void AABB::merge(FXMVECTOR minPoint, FXMVECTOR maxPoint)
	{
		/// For minpoint
		mMinPoint.x = XMMin(XMVectorGetX(minPoint), mMinPoint.x);
		mMinPoint.y = XMMin(XMVectorGetY(minPoint), mMinPoint.y);
		mMinPoint.z = XMMin(XMVectorGetZ(minPoint), mMinPoint.z);

		/// For maxpoint
		mMaxPoint.x = XMMax(XMVectorGetX(maxPoint), mMaxPoint.x);
		mMaxPoint.y = XMMax(XMVectorGetY(maxPoint), mMaxPoint.y);
		mMaxPoint.z = XMMax(XMVectorGetZ(maxPoint), mMaxPoint.z);

		calcCenterPoint();

		mIsInvalid = false;
	}
	//---------------------------------------------------------------------
	void AABB::addPoint(FXMVECTOR point)
	{
		if (XMVector3NearEqual(point, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.01f, 0.01f, 0.01f, 0.01f)))
			return;

		/// For minpoint
		mMinPoint.x = XMMin(XMVectorGetX(point), mMinPoint.x);
		mMinPoint.y = XMMin(XMVectorGetY(point), mMinPoint.y);
		mMinPoint.z = XMMin(XMVectorGetZ(point), mMinPoint.z);

		/// For maxpoint
		mMaxPoint.x = XMMax(XMVectorGetX(point), mMaxPoint.x);
		mMaxPoint.y = XMMax(XMVectorGetY(point), mMaxPoint.y);
		mMaxPoint.z = XMMax(XMVectorGetZ(point), mMaxPoint.z);

		calcCenterPoint();

		mIsInvalid = false;
	}
	//---------------------------------------------------------------------
	void AABB::calcCenterPoint()
	{
		mCenterPoint.x = (mMinPoint.x + mMaxPoint.x) / 2.0f;
		mCenterPoint.y = (mMinPoint.y + mMaxPoint.y) / 2.0f;
		mCenterPoint.z = (mMinPoint.z + mMaxPoint.z) / 2.0f;
	}
	//---------------------------------------------------------------------
	void AABB::move(FXMVECTOR trans)
	{
		XMStoreFloat3(&mMinPoint, XMLoadFloat3(&mMinPoint) + trans);
		XMStoreFloat3(&mMaxPoint, XMLoadFloat3(&mMaxPoint) + trans);

		calcCenterPoint();
	}
	//---------------------------------------------------------------------
	void AABB::rotate(FXMVECTOR quat)
	{
		XMStoreFloat3(&mMinPoint, XMVector3Rotate(XMLoadFloat3(&mMinPoint), quat));
		XMStoreFloat3(&mMaxPoint, XMVector3Rotate(XMLoadFloat3(&mMaxPoint) , quat));

		calcCenterPoint();
	}
	//---------------------------------------------------------------------
	void AABB::reset()
	{
		mMinPoint.x = mMinPoint.y = mMinPoint.z = 0.0f;
		mMaxPoint.x = mMaxPoint.y = mMaxPoint.z = 0.0f;
		mCenterPoint.x = mCenterPoint.y = mCenterPoint.z = 0.0f;

		mIsInvalid = true;
	}

}