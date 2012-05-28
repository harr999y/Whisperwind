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

		calcCenterAndHalfSize();
	}
	//---------------------------------------------------------------------
	void AABB::merge(const AABBPtr & aabb)
	{
		IF_NULL_RETURN(aabb);

		IF_FALSE_RETURN(!aabb->getIsInvalid());

		merge(aabb->getMinPoint(), aabb->getMaxPoint());
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

		calcCenterAndHalfSize();

		mIsInvalid = false;
	}
	//---------------------------------------------------------------------
	void AABB::addPoint(FXMVECTOR point)
	{
		if (mIsInvalid)
		{
			XMStoreFloat3(&mMinPoint, point);
			XMStoreFloat3(&mMaxPoint, point);

			mIsInvalid = false;

			return;
		}

		/// For minpoint
		mMinPoint.x = XMMin(XMVectorGetX(point), mMinPoint.x);
		mMinPoint.y = XMMin(XMVectorGetY(point), mMinPoint.y);
		mMinPoint.z = XMMin(XMVectorGetZ(point), mMinPoint.z);

		/// For maxpoint
		mMaxPoint.x = XMMax(XMVectorGetX(point), mMaxPoint.x);
		mMaxPoint.y = XMMax(XMVectorGetY(point), mMaxPoint.y);
		mMaxPoint.z = XMMax(XMVectorGetZ(point), mMaxPoint.z);

		calcCenterAndHalfSize();

		mIsInvalid = false;
	}
	//---------------------------------------------------------------------
	void AABB::calcCenterAndHalfSize()
	{
		XMVECTOR minPoint = XMLoadFloat3(&mMinPoint);
		XMVECTOR maxPoint = XMLoadFloat3(&mMaxPoint);

		XMStoreFloat3(&mCenterPoint, (minPoint + maxPoint) * 0.5f);
		XMStoreFloat3(&mHalfSize, (maxPoint - minPoint)  * 0.5f);
	}
	//---------------------------------------------------------------------
	void AABB::move(FXMVECTOR trans)
	{
		XMStoreFloat3(&mMinPoint, XMLoadFloat3(&mMinPoint) + trans);
		XMStoreFloat3(&mMaxPoint, XMLoadFloat3(&mMaxPoint) + trans);

		calcCenterAndHalfSize();
	}
	//---------------------------------------------------------------------
	void AABB::rotate(FXMVECTOR quat)
	{
		XMVECTOR oldMin = XMLoadFloat3(&mMinPoint);
		XMVECTOR oldMax = XMLoadFloat3(&mMaxPoint);
		XMVECTOR point;

		this->reset();

		point = oldMin;
		this->addPoint(XMVector3Rotate(point, quat));

		point = XMVectorSetZ(point, XMVectorGetZ(oldMax));
		this->addPoint(XMVector3Rotate(point, quat));

		point = XMVectorSetY(point, XMVectorGetY(oldMax));
		this->addPoint(XMVector3Rotate(point, quat));

		point = XMVectorSetZ(point, XMVectorGetZ(oldMin));
		this->addPoint(XMVector3Rotate(point, quat));

		point = XMVectorSetX(point, XMVectorGetX(oldMax));
		this->addPoint(XMVector3Rotate(point, quat));

		point = XMVectorSetZ(point, XMVectorGetZ(oldMax));
		this->addPoint(XMVector3Rotate(point, quat));

		point = XMVectorSetY(point, XMVectorGetY(oldMin));
		this->addPoint(XMVector3Rotate(point, quat));

		point = XMVectorSetZ(point, XMVectorGetZ(oldMin));
		this->addPoint(XMVector3Rotate(point, quat));

		calcCenterAndHalfSize();
	}
	//---------------------------------------------------------------------
	void AABB::reset()
	{
		mMinPoint.x = mMinPoint.y = mMinPoint.z = 0.0f;
		mMaxPoint.x = mMaxPoint.y = mMaxPoint.z = 0.0f;
		mCenterPoint.x = mCenterPoint.y = mCenterPoint.z = 0.0f;
		mHalfSize.x = mHalfSize.y = mHalfSize.z = 0.0f;

		mIsInvalid = true;
	}

}