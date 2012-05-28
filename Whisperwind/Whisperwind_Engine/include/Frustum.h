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
#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#include "Util.h"
#include "MathDefine.h"

namespace Engine
{
	enum VisibilityType
	{
		VT_FULL,
		VT_PARTIAL,
		VT_NONE
	};

	class WHISPERWIND_API Frustum
	{
	public:
		Frustum(Util::real nearCilp, Util::real farClip, Util::real width, Util::real height);

		~Frustum()
		{}

	private:
		enum FrustumSide
		{
			FS_TOP,
			FS_BUTTOM,
			FS_LEFT,
			FS_RIGHT,
			FS_FRONT,
			FS_BACK,
			FS_COUNT
		};

		enum LocateSide
		{
			LS_POSITIVE,
			LS_NEGATIVE,
			LS_INTERSECT
		};

	public:
		void setViewParams(FXMVECTOR pos, FXMVECTOR lookAt, FXMVECTOR upDir);
		XMMATRIX getViewMatrix() const { return XMLoadFloat4x4(&mViewMatrix); };
		XMMATRIX getViewProjMatrix() const { return XMLoadFloat4x4(&mViewProjMatrix); };

		VisibilityType isVisible(const Util::AABBPtr & aabb);

	private:
		LocateSide testContainState(FXMVECTOR plane, const Util::AABBPtr & aabb);

	private:
		Util::real mNearClip;
		Util::real mFarClip;
		Util::real mAspect;

		XMFLOAT4X4 mProjMatrix;
		XMFLOAT4X4 mViewMatrix;
		XMFLOAT4X4 mViewProjMatrix;

		XMFLOAT4 mPlanes[FS_COUNT];

	private:
		DISALLOW_COPY_AND_ASSIGN(Frustum);
	};

}

#endif