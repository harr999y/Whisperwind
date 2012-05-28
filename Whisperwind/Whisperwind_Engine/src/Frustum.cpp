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
#include "Frustum.h"

namespace Engine
{
	//---------------------------------------------------------------------
	Frustum::Frustum(Util::real nearCilp, Util::real farClip, Util::real width, Util::real height) :
        mNearClip(nearCilp),
		mFarClip(farClip)
	{
		mAspect = width / height;

		XMStoreFloat4x4(&mProjMatrix, XMMatrixPerspectiveFovLH(XM_PI / 2.0, mAspect, mNearClip, mFarClip));
	}
	//---------------------------------------------------------------------
	void Frustum::setViewParams(FXMVECTOR pos, FXMVECTOR lookAt, FXMVECTOR upDir)
	{
		XMStoreFloat4x4(&mViewMatrix, XMMatrixLookAtLH(pos, lookAt, upDir));
		XMStoreFloat4x4(&mViewProjMatrix, XMLoadFloat4x4(&mViewMatrix) * XMLoadFloat4x4(&mProjMatrix));

		/// front
		mPlanes[FS_FRONT].x = mViewProjMatrix._14 + mViewProjMatrix._13;
		mPlanes[FS_FRONT].y = mViewProjMatrix._24 + mViewProjMatrix._23;
		mPlanes[FS_FRONT].z = mViewProjMatrix._34 + mViewProjMatrix._33;
		mPlanes[FS_FRONT].w = mViewProjMatrix._44 + mViewProjMatrix._43;
		XMStoreFloat4(&mPlanes[FS_FRONT], XMPlaneNormalizeEst(XMLoadFloat4(&mPlanes[FS_FRONT])));

		/// back
		mPlanes[FS_BACK].x = mViewProjMatrix._14 - mViewProjMatrix._13;
		mPlanes[FS_BACK].y = mViewProjMatrix._24 - mViewProjMatrix._23;
		mPlanes[FS_BACK].z = mViewProjMatrix._34 - mViewProjMatrix._33;
		mPlanes[FS_BACK].w = mViewProjMatrix._44 - mViewProjMatrix._43;
		XMStoreFloat4(&mPlanes[FS_BACK], XMPlaneNormalizeEst(XMLoadFloat4(&mPlanes[FS_BACK])));

		/// left
		mPlanes[FS_LEFT].x = mViewProjMatrix._14 + mViewProjMatrix._11;
		mPlanes[FS_LEFT].y = mViewProjMatrix._24 + mViewProjMatrix._21;
		mPlanes[FS_LEFT].z = mViewProjMatrix._34 + mViewProjMatrix._31;
		mPlanes[FS_LEFT].w = mViewProjMatrix._44 + mViewProjMatrix._41;
		XMStoreFloat4(&mPlanes[FS_LEFT], XMPlaneNormalizeEst(XMLoadFloat4(&mPlanes[FS_LEFT])));

		/// right
		mPlanes[FS_RIGHT].x = mViewProjMatrix._14 - mViewProjMatrix._11;
		mPlanes[FS_RIGHT].y = mViewProjMatrix._24 - mViewProjMatrix._21;
		mPlanes[FS_RIGHT].z = mViewProjMatrix._34 - mViewProjMatrix._31;
		mPlanes[FS_RIGHT].w = mViewProjMatrix._44 - mViewProjMatrix._41;
		XMStoreFloat4(&mPlanes[FS_RIGHT], XMPlaneNormalizeEst(XMLoadFloat4(&mPlanes[FS_RIGHT])));

		/// top
		mPlanes[FS_TOP].x = mViewProjMatrix._14 - mViewProjMatrix._12;
		mPlanes[FS_TOP].y = mViewProjMatrix._24 - mViewProjMatrix._22;
		mPlanes[FS_TOP].z = mViewProjMatrix._34 - mViewProjMatrix._32;
		mPlanes[FS_TOP].w = mViewProjMatrix._44 - mViewProjMatrix._42;
		XMStoreFloat4(&mPlanes[FS_TOP], XMPlaneNormalizeEst(XMLoadFloat4(&mPlanes[FS_TOP])));

		/// buttom
		mPlanes[FS_BUTTOM].x = mViewProjMatrix._14 + mViewProjMatrix._12;
		mPlanes[FS_BUTTOM].y = mViewProjMatrix._24 + mViewProjMatrix._22;
		mPlanes[FS_BUTTOM].z = mViewProjMatrix._34 + mViewProjMatrix._32;
		mPlanes[FS_BUTTOM].w = mViewProjMatrix._44 + mViewProjMatrix._42;
		XMStoreFloat4(&mPlanes[FS_BUTTOM], XMPlaneNormalizeEst(XMLoadFloat4(&mPlanes[FS_BUTTOM])));
	}
	//---------------------------------------------------------------------
	VisibilityType Frustum::isVisible(const Util::AABBPtr & aabb)
	{
		if (!aabb || aabb->getIsInvalid())
			return VT_NONE;

		bool isIntersect = false;
		for (Util::u_int planeIt = 0; planeIt < FS_COUNT; ++planeIt)
		{
			LocateSide ls = testContainState(XMLoadFloat4(&mPlanes[planeIt]), aabb);

			if (LS_NEGATIVE == ls)
				return VT_NONE;

			if (LS_INTERSECT == ls)
				isIntersect = true;
		}

		if (!isIntersect)
			return VT_FULL;
		else
			return VT_PARTIAL;
	}
	//---------------------------------------------------------------------
	Frustum::LocateSide Frustum::testContainState(FXMVECTOR plane, const Util::AABBPtr & aabb)
	{
		Util::real dist = XMVectorGetX(XMPlaneDotCoord(plane, aabb->getCenterPoint()));

		XMVECTOR halfSize = aabb->getHalfSize();
		Util::real maxAbsDist = abs(XMVectorGetX(plane) * XMVectorGetX(halfSize)) + 
			abs(XMVectorGetY(plane) * XMVectorGetY(halfSize)) +
			abs(XMVectorGetZ(plane) * XMVectorGetZ(halfSize));

		if (dist < -maxAbsDist)
			return LS_NEGATIVE;

		if (dist > + maxAbsDist)
			return LS_POSITIVE;

		return LS_INTERSECT;
	}

}